# P3 GPU 메모리 후속 결과

2026-11-05. **생산 Controller/Core의 GPU 메모리 종합 검사와 재현한 reflection 실패 수정을 완료했다. P3 전체는 진행 중이다.** [작은 증거](evidence/p3-gpu-memory/README.md).

## 구현

1. 기본 OFF인 `DALI_WEB_TEST_MEMORY`를 추가했다. 실제 host의 render pthread에서 생산 Controller·Core GpuBuffer/UniformBufferV2를 실행하고, FrameDone mailbox 뒤 main에서 검사 결과를 수집한다. GL 응답 대체·mock·빈 GL·undefined 억제는 없다. 기존 readback 옵션과 계측 없는 생산 경로를 유지한다.
2. GPU 버퍼는 WebGL2 `getBufferSubData`로 실제 내용을 읽는다. CPU uniform은 실제 생산 Program의 standalone uniform 업로드 후 `glGetUniformfv`로 검사하고, DALi scene의 uniform 색 변경도 두 프레임 픽셀로 확인한다. CPU shadow나 Core bool만으로 성공을 판정하지 않는다.
3. 공통 GLES reflection은 기존 Controller `CanSubmit()`을 통해 GL 조회 실패 후 처리를 중단한다. attribute/uniform 조회 결과, block index·이름·길이를 실패 상태에서 사용하지 않고, Program 생성도 reflection 실패 뒤 uniform cache 구성을 중단한다. 공개 Graphics API·native Memory2/3·native 재시도 정책은 유지한다. native CanSubmit은 기존처럼 true다.
4. native 회귀 스크립트에 reflection을 추가하고, host compiler가 있으면 실제 native compile/nm도 실행한다. native configure는 pkg-config를 켜서 정상 의존성 탐색을 진행한다.

## 검증

| 검사 | 실제 근거·결과 |
|---|---|
| SDK/생산 빌드 | 전용3.1.24 lock/profile, EMCC_CORES=6·최대-j6. final-001 Core333 완료, 같은 Core archive·입력 drift0으로 final-002 생산35 compile/nm·strict 링크0. 최종499개 입력 drift0 |
| 16-byte·중간4-byte | Core GpuBuffer RETAIN 초기16바이트, callback 반환0/4에서 offset6의4바이트 변경과 앞6/뒤6 보존. 반환16에서 전체16바이트 변경. 무쓰기 callback0도 이전 데이터 보존 |
| nonzero offset | map offset3/size10 + lock offset3/size4 → 실제 buffer offset6. Unlock(false) 뒤 Unmap의 전체 매핑 보존·업로드 확인 |
| CPU uniform | Core persistent mapping 동일 크기·증가,16바이트 vec4의 offset4 중간4바이트 변경. 실제 생산 Program upload 후 GL uniform값 전체16바이트 일치. scene 두 번째 프레임 색 변경도 확인 |
| GPU uniform | Core 두 GPU buffer 회전·16→32 증가·부분 Write·Flush, 제출 buffer를 rotation 전에 저장한 뒤 실제 buffer readback. 두 프레임 반복 |
| grow/recycle·수명 | 일반 allocator의 mapped 객체 재사용 거부·unmapped 재사용, 새 generation zero·32바이트 grow와 업로드. detach 후 GL 객체 삭제·shadow pin, dirty Memory의 늦은 해제에도 새 GPU buffer 오염 없음. custom allocator의 마지막 참조 해제·전체 allocation/free 일치 |
| Firefox157.0.1 | 직접 헤더/SW 각각 두 프레임115개 검사 통과, 그중46개 GPU buffer/uniform readback. GL error0, 오류 목록 없음, ready→stopped. 배경 `[13,25,51,255]`, 사각형1 `[229,76,25,255]`, 사각형2 `[51,153,204,255]` |
| SW | `/probe/` 최초 isolation=false → 제어/reload 후 true, sharedMemory/controlled=true. 운영 캐시·재방문 검사는 아님 |
| 실제 실패 | malformed shader와 reflection 중 실제 invalid GL enum 각각 ready→failure→stopped. 수정 전 reflection은 Wasm index out of bounds·stopped 없음, 수정 후 trap 없음 |
| 계측 없음 | 동일 최종 생산 객체 + readback/memory define 없는 host 별도 compile/strict 링크0. Firefox 직접 헤더 오류 없이 ready→stopped |
| 공통/native | GLES/VULKAN/DYNAMIC source·Web closure·독립 generated 경로 통과. native configure3개0, 관련14개 GNU11.4 compile/nm0 및 같은14개 Emscripten compile/nm0 |

GPU46개와 CPU 반환값·객체/참조 검사69개를 구분한다. `Unlock/Unmap`은 void이고 Core bool이 GPU 성공을 뜻하지 않는다. 여기서는 별도 readback·GL 오류와 실제 픽셀을 근거로 판정했다. shader 오류의 성공 판정은 정상 렌더 성공이 아니라 예상된 failure/stop 처리다.

## 특이 사항

1. reflection 중 실제 GL 오류를 발생시키면 wrapper가 이후 조회를 생략하면서 zero-filled block index를 유효한 결과로 사용해 trap이 발생했다. 공통 내부 처리에 기존 제출 gate를 적용해 해결했다. native의 API·재시도 정책을 변경하지 않았다. 모든 GL 오류 위치·context loss·OOM의 안전성을 증명한 것은 아니다.
2. custom allocator는 기존 factory가 recycling보다 먼저 새 객체를 생성한다. 처음 수명 테스트의 객체 동일성 가정이 실패했으며 생산 정책을 바꾸지 않고 일반/custom allocator를 나누어 검사했다. 중간 `browser-memory-003`은 실패1로 보존했다.
3. 새 테스트의 shader state 참조 형식 오류로 `gpu-memory-002` host compile이 실패했다. 수정 후 통과했다. `gpu-memory-final-001` 빌드 중 테스트 header의 코드 스타일 정리가 있어 해당 source hash drift를 확인했다. final-002에서 마지막 소스를 고정하고 생산35개를 다시 컴파일했다. Core 입력/아카이브는 그대로이며 final-002 drift0이다. 중간 결과는 최종 증거로 사용하지 않는다.
4. 현재 native compiler는 존재한다. 이전 compiler 부재와 달리 configure3개는 통과했지만 전체 Adaptor 빌드는 설치된 `dali/integration-api/debug.h` 등 Core 헤더를 찾지 못해 모두 실패1이다. configure OFF의 앞선 시도는 `PKG_CHECK_MODULES` 미정의로 실패했고, 스크립트를 ON으로 수정해 실제 의존성 탐색을 실행했다. native 전체 build/install/runtime은 미검증이다.
5. 시작 HEAD와 읽기 조회한 origin/main은 모두 `04a686e9ac70c6a380ff045ed635723f8861d108`, 작업 트리 clean이었다. 현재 변경은 미커밋이다. 원격 읽기 첫 시도의 DNS 제한은 승인된 sandbox 확장으로 해소했다. commit/push·upstream 갱신·운영 서버·Pages 배포는 하지 않았다. 검사한 기존748개 lock/산출물 hash 변경0이다.

## 미검증·단계 상태

이번 GPU 메모리 행렬과 재현한 shader/reflection 실패 단위를 완료했다. P3 전체 완료나 P4 진입 인수, 전체 자원 회수·사용자 코드 격리 완료를 선언하지 않는다.

1. P3/P4 이벤트 Worker 플레이그라운드 격리 조기 검사 유지. P4 종료 전 무한 루프 중 편집기 응답성·전체 pthread/Canvas 종료·새 Module/Canvas 재생성 검증 필요.
2. SW 재방문·업데이트·해제/실패·캐시 정책, 별도 승인 실제 Pages 검증·배포 유지. GitHub Pages+SW 우선이며 다른 관리형 호스팅으로 전환하지 않는다.
3. Chromium/타 OS, context loss·전역 OOM, 모든 실패 위치·전체 worker/resource 누수, native runtime은 미검증. 현 GPU 검사는 로컬 Firefox 실제 WebGL 실행이며 물리 GPU 가속/성능을 측정하지 않았다. readback 동기화 비용 때문에 성능 근거로 사용하지 않는다.
4. 원본 Emception+Emscripten3.1.24, 서버 없는 브라우저 C++ 컴파일 방향 유지. 기존6.0.13 SDK·lock·산출물 및 P0/과거 증거는 보존하며 혼용하지 않는다.
