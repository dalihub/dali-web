# P2-b Core 실패 처리 중간 결과

2026-10-23 (Asia/Seoul). 사용자 설계 승인 후 Core 내부 실패 전파 단위를 구현·검증했다. **웹 메모리·실제 host·생산 strict 링크는 아직 미완료이며 전체 P2-b·상위 P2도 미완료다.** 검증 가능한 구현 단위에서 남기는 중간 인수인계다. 원래 실행 범위와 승인은 유지한다.

시작/현재 HEAD는 `abf62214b23c651f06536138cb8b6e0c0fddbfb7`. 이번 변경은 working tree에 있으며 commit/push하지 않았다. 두 abandon 패치(358295, 357659)를 가져와 적용하지 않았으며 이 구현은 그 패치에 의존하지 않는다. [승인 전 조사](P2-b-MEMORY-BLOCKER.md)는 역사 기록으로 보존한다.

## 승인한 정책과 구현 범위

1. 공개 `Graphics::Controller`/`Memory`의 시그니처와 vtable을 유지한다. Core internal GpuBuffer 업로드 결과를 bool로 전달하고 create/map/lock 실패에서 복사·callback 실행을 막는다. lock 실패 뒤 Memory를 해제한다. 성공한 크기·유효성만 확정하고 callback의 정상 0-byte 반환과 실패를 구분한다.
2. vertex/index는 실패한 업로드의 dirty를 유지한다. callback lock은 조기 반환에서도 해제한다. Geometry는 upload가 유효하지 않으면 bind와 Draw 모두 거부해 기존 binding cache를 거치는 draw도 막는다. callback 결과가 용량을 넘으면 유효 draw로 취급하지 않는다. 사용자 callback 자체의 범위 밖 쓰기를 막는 sandbox를 추가한 것은 아니다.
3. CPU/GPU uniform은 view·buffer 범위, null 데이터, unsigned overflow, 정렬 padding과 map/lock을 검사한다. 준비 실패를 latch하고 `Write`/`Flush`에 반영한다. view 생성 실패도 해당 UBO를 무효화한다. 장면 합계는 uint64_t로 누적한 뒤 uint32_t 범위를 확인한다. 명시적 다음 준비는 Core에서 허용하되 Web backend의 영구 failure latch 정책은 별도로 연결해야 한다.
4. **특이 사항: 공유 UBO는 이미 기록한 다른 draw도 참조한다.** Renderer의 반환만으로 충분하지 않아 scene/offscreen·onscreen pass 단위의 최종 제출 전 검사를 추가했다. uniform 실패면 기록된 command buffer를 닫고 reset하며 제출하지 않는다. sync 생성·RenderTracker 소비·readback 등록은 검사가 성공한 뒤로 옮겼다. 이전에 제출한 다른 장면까지 되돌리는 프레임 rollback은 아니다.
5. 실제 Web backend는 allocation/map/context/fence 오류를 latch하고 후속 제출을 차단한 뒤 host의 비동기 failure/stop으로 연결해야 한다. 실패 후 새 Worker/실행 인스턴스를 사용한다. 이 Web latch와 host 연결은 **아직 구현하지 않았다**. `Unlock`/`UnmapMemory`가 void이므로 현재 Core의 bool 성공은 CPU 준비 성공이며 GL 업로드/GPU 완료를 뜻하지 않는다.

변경은 `libs/dali-core/dali/internal/render/{renderers,common}`에 한정된다. Adaptor·native Memory2/3·Vulkan 생산 구현은 이번 단위에서 수정하지 않았다. Core는 모든 backend의 공통 경로이므로 native 회귀 미검증 범위도 이 render 경로 전체에 해당한다.

## 검사와 결과

실제 Core에 테스트 전용 Graphics Controller/Memory 실패를 주입한다. 이 테스트는 생산 링크·GPU 보존 검증이 아니다. 새 테스트는 `tests/unit/core-buffer-failure.cpp`, 별도 CMake는 `cmake/p2-b-core-tests`, 재현 스크립트는 `scripts/check-core-buffer-failure.py`다.

| 실행 | 결과 | 의미·한계 |
|---|---|---|
| `core-failure-005` | configure/build/Node failure/Node smoke 각각 exit 0, 빌드 375단계 | 실제 pthread Core + 테스트 전용 mock graphics |
| GpuBuffer·Geometry | create/map/lock 실패, callback 미호출·0/4/16/17-byte 반환, dirty·lock 해제·bind/draw 차단 통과 | callback 반환 수와 실패 안전성 검사. GPU 데이터 보존 검사가 아님 |
| CPU/GPU uniform | create/map/lock 실패, 명시적 재준비, nonzero view offset, bounds·UINT32_MAX·alignment 검사 통과 | CPU 주입 검사. 실제 Web map/lock offset 검사는 남음 |
| 실제 Core scene | uniform create/map/lock/view write 범위 실패 때 제출·offscreen sync 생성·readback 결과 없음, 정상 경로 제출·sync 생성 통과 | mock Controller 사용. 실제 GPU 완료나 native 실행 아님 |
| 기존 Core smoke | 애니메이션 midpoint 50 / endpoint 100, exit 0 | mock graphics 정상 경로 회귀 |
| `core-failure-sources-001` | source 회귀 exit 0 | GLES/VULKAN/DYNAMIC 기존 source 선택 유지, Vulkan 목록 동일 |
| `core-failure-native-001` | native configure GLES/VULKAN/DYNAMIC 각각 exit 1 | host C/C++ compiler 부재. native Core/Adaptor build/install/runtime 미검증 |
| `core-failure-web-001` | Core 333단계·34개 compile/nm 각각 0, strict 기본/FULL_ES3 각각 1·10/6개 미해결 | map 없음, 기존 경계와 동일. 실제 host 없이 부분 생산 객체 진단 |
| 같은 native 검사 | context/rendering/sync/sync-pool/egl-graphics 5개 compile/nm 각각 exit 0 | Emscripten 교차 컴파일이며 native 실행 증거 아님 |

`core-failure-001/002`는 테스트 코드와 log macro include 컴파일 오류로 build exit 1. 수정 후 `003`은 통과했다. offscreen 검사를 추가한 `004`는 정상 경로의 sync 기대 검사에서 Node exit 1: 테스트 RenderTask에 `REQUIRES_SYNC` 설정이 빠져 있었다. 이를 명시한 `005`가 최종 통과이며 앞선 실패 로그·산출물은 보존한다. 대규모 upstream 전체 자동 테스트는 실행하지 않았다.

명령·개별 종료 코드·버전·입력 hash·보존 검사는 [증거](evidence/p2-b-core-failure/README.md)에 있다. build/log는 별도 경로, 최대 -j6이다. SDK·산출물·대형 로그를 commit하지 않는다.

## 다음 구현과 남은 게이트

1. 추가 승인 대기 없이 승인된 웹 메모리 구현을 이어갈 수 있다. Controller MapBufferRange factory를 native/Web 파일로 나누고 WebBufferMemory와 필요한 버퍼 소유 CPU shadow를 연결한다. native Memory2/3 유지, Memory4 없음. buffer 생성 실패의 재시도 큐·discard 순서와 Memory/Buffer 종료 수명을 함께 처리해야 한다. native 자동 재시도 정책을 Web fail-stop으로 함께 바꾸지 않는다.
2. 16-byte 패턴의 중간 4-byte 갱신과 앞뒤 보존, callback 0/부분/전체, nonzero map/lock offset, CPU/GPU uniform, bounds/overflow/할당 실패, double unmap, grow/recycle·종료 해제를 확인한다. 이번 Core 테스트로 Web 저장소나 GPU 보존을 통과 처리하지 않는다.
3. wrapper/capability → shader cache 경계 → 실제 D3 host → 기본 WebGL2 생산 strict 링크. 정식 source closure와 generated source 정리, production source manifest/nm/map·개별 종료 코드가 남았다. WEB production configure 거부 유지. mock·빈 GL/EGL/sync·undefined 억제로 완료 처리 금지.
4. P3 실제 GPU 픽셀·업로드 보존, 최초 pthread/browser 로컬 헤더/SW 비교, 별도 승인 실제 Pages 검증, P3/P4 격리 조기 검증과 P4 종료 전 무한 루프 응답성·전체 pthread/Canvas 종료/재생성을 유지한다. upstream 갱신·배포·서버 구축 없음.

승인 범위를 넘는 공통/public 계약 변경·전체 플랫폼 포팅·실험 확대·합의 방향 변경이 필요하면 다시 중단·보고한다. 현재 구현 단위는 재개 가능한 중간 결과이며 전체 P2 완료 선언이 아니다.
