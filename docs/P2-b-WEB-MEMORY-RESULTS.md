# P2-b Web 메모리 중간 구현

최신 상태(2026-10-23): [생산 구현·strict 링크 완료 결과](P2-b-PRODUCTION-RESULTS.md). 아래 내용은 이전 checkpoint 기록이며 현재 승인 대기나 남은 생산 링크 상태를 뜻하지 않는다. 실제 browser/GPU 인수는 P3/P4에 남는다.

2026-10-23 (Asia/Seoul). **웹 메모리 저장소·매핑의 CPU 검사 지점에서 남기는 중간 인수인계다. 단계 4의 생산 Controller 통합 검증과 host 비동기 failure/stop 연결, 전체 P2-b·상위 P2는 미완료다.** 원래 실행 범위와 기존 승인은 유지한다. 추가 승인 대기 상태가 아니다.

시작 HEAD와 현재 HEAD는 `abf62214b23c651f06536138cb8b6e0c0fddbfb7`. 기존 Core internal 변경·테스트·문서를 보존하고 Adaptor 변경을 추가했다. GitHub 연결의 읽기 전용 조회에서 원격 main은 `a68e3f00f0c7752561137c179500b004b930285e`. commit/push·upstream 갱신 없음.

## 구현 경계

1. `MapBufferRange`를 `native-gles-memory.cpp` / `web-gles-memory.cpp`로 분리했다. native GLES 버전별 Memory2/3 선택은 그대로이며 Memory4는 없다. 공개 Graphics Controller/Memory 시그니처·vtable은 변경하지 않았다.
2. Buffer 초기화·recycle·해제는 `native-gles-buffer.cpp` / `web-gles-buffer.cpp`에서 선택한다. 공통 생성자·Bind는 유지한다. native 원래 메서드 본문과 Memory2/3 소스는 보존했다. 공통 Buffer에 Web 저장소 포인터 한 개가 추가되지만 native에서는 null이며 shadow를 할당하지 않는다.
3. Web GPU 버퍼는 처음 매핑할 때 zero 초기화 CPU shadow를 할당한다. 매핑되지 않은 GPU 버퍼는 shadow가 없다. MapBufferInfo에 전체 overwrite/callback 구분이 없으므로 **매핑된 GPU 버퍼 모두** 보존 가능한 저장소가 필요하다. CPU uniform은 별도 사본 없이 같은 저장소를 사용한다. 최초 shadow 생성 시 GPU에도 전체 zero 구간을 업로드하고, 이후 map 구간만 BufferSubData한다.
4. lock offset은 map 시작점 기준이다. map/lock 크기는 뺄셈으로 overflow 없이 검사한다. READ mapping·중복 map/lock·0 크기는 거부한다. callback 반환 count는 dirty 위치가 아니므로 보수적으로 map 구간 전체를 업로드한다. Unlock(false) 뒤 unmap은 pending write를 반영하고, Unlock(true) 뒤 중복 unlock/unmap은 재업로드하지 않는다. 잠긴 상태의 Flush 뒤 추가 쓰기도 다음 unlock에서 반영한다.
5. 버퍼가 저장소 참조 하나를 소유하고 Memory가 map 동안 추가 참조를 보유한다. Buffer detach 뒤에는 업로드하지 않고 마지막 Memory가 CPU 할당을 해제한다. 별도 shared_ptr 제어 블록 할당 없이 render-owner 참조 카운트를 쓴다. 사용자 allocator의 함수는 복사하지만 userData는 외부 소유이므로 마지막 매핑 해제까지 유효해야 한다. raw lock 포인터를 다른 스레드에 넘기는 계약을 추가한 것은 아니다.
6. 같은 크기 recycle은 매핑 중이면 거부한다. DISCARD/orphan은 새 zero 세대로 시작하고 이전 byte 보존을 약속하지 않는다. 크기가 다르면 기존 factory가 새 Buffer를 만든다. recycle 전 저장소 revision을 Buffer 변경 번호에 누적해 CPU uniform cache의 값 충돌을 피한다.
7. `WebFailure`는 GL owner의 allocation/map/GL/context/fence 실패를 영구 latch한다. `CanSubmit`은 submit·flush·각 command 처리 앞에서 검사한다. 실패 후 생성 큐는 재시도하지 않고 명령을 건너뛴다. native hook은 기존 실행·재시도 정책을 유지한다. queue의 static 임시 저장소를 호출별로 바꾸고 discard 전 create 큐 참조를 제거한다. factory의 null allocation은 placement-new 전에 검사한다. 실패 취소 경로에서 대기 texture staging도 해제한다.
8. Web sync/surface 종료는 stopped 상태를 게시해 늦은 Memory 파괴가 GL을 호출하지 않도록 한다. failure/stop 상태는 동일 Worker에서 reset하지 않는다. 실제 host는 아직 없으므로 `WebFailure::Reason()`을 owner에서 읽어 main으로 비동기 게시하고 Worker·Canvas를 종료하는 연결은 **미구현**이다. Controller gate의 실제 제출 차단·종료 순서는 아직 런타임으로 검증하지 않았다.

Unlock/UnmapMemory는 여전히 void다. Core bool 성공은 CPU 준비 결과일 뿐 GL 업로드 성공이나 GPU 완료가 아니다. 실패 전에 제출한 scene/pass를 되돌리는 rollback도 추가하지 않았다.

## 검사와 한계

[명령·개별 종료 코드·입력 hash·nm·링크 증거](evidence/p2-b-web-memory/README.md). 전체 로그와 build는 새 `memory-*` 디렉터리에 보존하며 최대 -j6이다.

- `memory-web-003`: pthread Core 333단계 및 생산 객체 37개 compile/nm 각각 exit 0. strict 기본 WebGL2/FULL_ES3 각각 exit 1, 미해결 10/6개, map 없음. GL wrapper의 mapping/BlendBarrier와 binary cache utility 의존이 남는다. link-only harness이며 실제 host 없음.
- `memory-unit-003`: compile/link와 Node 12개 모드 모두 exit 0. 실제 WebBufferMemory/Storage/Failure/Fence 코드에 GL 응답을 주입했다. 생산 Controller나 실제 GPU를 사용한 검사가 아니다.
- 16-byte 패턴의 map(4,8)·lock(2,4) 갱신, 앞뒤 byte 보존, callback 0/부분/전체에 해당하는 쓰기, CPU uniform 저장소의 GL 업로드 없음, 중복 unlock, 잠긴 Flush 뒤 추가 쓰기, map/lock bounds·UINT32_MAX, 중복 map, shadow allocator 실패, GL upload/context/fence 실패 latch, detach/stop 뒤 늦은 해제 통과.
- 저장소 16→32 grow 및 같은 크기 새 세대는 CPU 단위에서 확인했다. **실제 Buffer::TryRecycle/CreateBuffer와 Core CPU/GPU uniform을 연결한 검사가 아니다.** 실제 buffer 생성/재활용·create/discard 큐·Controller 제출 차단·Memory를 보유한 전체 종료 검사는 남았다. storage 객체·Memory 객체 자체의 nothrow allocation 실패는 아직 주입하지 않았다.
- preserve 모드의 주입 upload 호출은 8회·120 byte, CPU 모드는 0회·0 byte다. 이는 특정 단위 테스트의 CPU 계수이며 실제 GPU 전송량·성능 측정이 아니다. GPU mapping은 버퍼 용량만큼 shadow, 최초 full zero upload 및 이후 map 범위 upload 비용을 갖는다.
- `memory-sources-001`: GLES/VULKAN/DYNAMIC source 회귀 exit 0, Vulkan 목록 동일. native 메모리/버퍼 추출 파일 2개가 추가됐다.
- `memory-native-002`: native configure 3개 각각 exit 1, host C/C++ compiler 부재. 추출 7개와 공통 Controller/Buffer·Memory2/3의 **11개 Emscripten compile/nm 각각 exit 0**. native build/install/runtime 증거가 아니며 수정한 공통 queue/factory·Buffer 상태/수명·환경 선택 전체의 native 실행은 미검증이다.
- `memory-web-001`: Web Buffer bit-field를 make_shared 전달 인자로 사용한 compile 실패. 보존했다. 이후 참조 카운트 구현으로 바꾸었다. `002`는 중간 입력의 compile 성공이며 최종 입력 증거는 `003`을 따른다.

`-sABORTING_MALLOC=0`을 단위/경계 링크에 명시해 명시적 malloc/nothrow 실패 경로를 사용할 수 있게 했다. 정식 host 링크에도 유지해야 한다. 이것이 모든 Core/STL 컨테이너의 전역 OOM까지 비동기 종료로 바꿨다는 뜻은 아니다.

## 다음 한 가지 작업과 유지할 전체 범위

다음은 **단계 4의 실제 생산 Buffer/Controller 통합 검사 보강**이다. create/discard 재시도 순서, GL buffer allocation 실패, grow/recycle, CPU/GPU uniform, Memory를 보유한 shutdown을 실제 생산 코드로 검증한다. CPU 단위의 저장소 세대 검사를 그 대신 쓰지 않는다. 이어 wrapper/capability → shader cache 경계 → 실제 D3 host/비동기 failure·ready·stop → 기본 WebGL2 strict 생산 링크 순서로 진행한다. host 연결이 끝나기 전 웹 메모리 전체 완료를 선언하지 않는다.

정식 source closure와 generated source 정리가 남았고 WEB production configure 거부를 유지한다. mock Controller·빈 GL/EGL/sync·undefined 억제로 생산 성공을 만들지 않는다. 실제 Core·Controller·자원·명령·host를 포함한 기본 WebGL2 strict exit 0과 manifest/nm/map이 상위 P2 게이트다. FULL_ES3 링크 제공은 runtime/데이터 보존 근거가 아니다.

P3 실제 GPU의 16-byte/중간 4-byte·callback·uniform·두 frame·grow/recycle·픽셀/readback, 최초 pthread/browser의 로컬 헤더/SW 비교, 별도 승인 Pages 검증, P3/P4 격리 조기 게이트, P4 종료 전 무한 루프 응답성과 전체 pthread/Canvas 종료·재생성을 유지한다. 브라우저 main 이벤트 + 별도 pthread update/upload/render, DOM 경계·main 비동기 ready/stop을 변경하지 않았다. 배포·서버 구축·다른 관리형 호스팅·SDK/lock 변경 없음.
