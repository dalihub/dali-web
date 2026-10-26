# P2-b 웹 메모리: Core 실패 처리 중단 당시 기록

현재 상태: 사용자 설계 승인(“오케이 진행”) 후 Core 내부 실패 전파를 구현·검증했다. [후속 결과](P2-b-CORE-FAILURE-RESULTS.md). 아래는 승인 전 조사·중단 당시 기록이며 현재 승인 대기 상태를 뜻하지 않는다.

2026-10-23 (Asia/Seoul). **웹 메모리 생산 구현 전 중단. 전체 P2-b·상위 P2 미완료.** 시작 HEAD는 `abf62214b23c651f06536138cb8b6e0c0fddbfb7`, main clean. GitHub 연결의 읽기 전용 ref 조회에서 원격 main은 `a68e3f00f0c7752561137c179500b004b930285e`다. commit·push·upstream 갱신 없음.

## 특이 사항과 중단 이유

P1에서 기록한 null 매핑 문제를 실제 Core 호출부까지 다시 추적했다. Web 메모리가 allocation/bounds 실패를 정직하게 반환하는 것만으로 호출부 안전성은 확보되지 않는다. 사용자의 “Core 호출부의 실패 처리에 공통 계약 변경이 필요하면 먼저 멈추고 보고” 조건에 따라, Core 내부 실패 전파·draw 처리 정책을 임의로 선택하지 않고 중단했다.

공개 Graphics API의 시그니처 변경이 반드시 필요하다는 판정은 아니다. `Controller::MapBufferRange`와 `Memory::LockRegion`은 이미 null 실패를 허용한다. 결정이 필요한 부분은 이를 소비하는 Core 내부 동작 계약이다. 소스 조사는 실패 경로의 결함을 보여 주지만 브라우저 crash나 GPU 손상을 실제 재현한 결과는 아니다.

## 확인한 호출 경로

경로는 `libs/dali-core/dali/` 기준이다. 줄 번호는 위 HEAD 기준이며 [소스 발췌·hash](evidence/p2-b-memory-contract/source-inspection.json)를 함께 확인한다.

| 경계 | 소스 근거 | 관찰과 영향 |
|---|---|---|
| map 실패 | `graphics-api/graphics-controller.h:300`, `graphics-api/graphics-memory.h:60` | 두 API 모두 실패 시 nullptr 허용. 성공 포인터·더미 저장소를 반환하는 우회는 계약을 지키지 못함 |
| 전체 업로드 | `internal/render/renderers/gpu-buffer.cpp:41` | size/capacity·graphics object 갱신 뒤 memory 무조건 역참조, lock 결과를 memcpy에 전달. map/lock 실패 검사 없음 |
| callback 업로드 | 같은 파일 `:78` | memory 무조건 역참조, lock 결과를 사용자 callback에 전달. null 전달과 성공적인 callback 0-byte 반환은 구분해야 함 |
| vertex 완료 처리 | `internal/render/renderers/render-vertex-buffer.cpp:74` | void 업로드 후 element count 갱신·dirty 해제. 하위에 단순 early return만 넣으면 업로드 실패를 반영하지 못함. callback 실행 구간에는 명시적 state lock도 있음 |
| index 완료 처리 | `internal/render/renderers/render-geometry.cpp:127` | void 업로드 뒤 `mIndicesChanged=false`. 실패 후 단순 반환만으로는 갱신 유실·잘못된 draw 방지를 보장하지 못함 |
| draw 제출 | 같은 파일 `:220`, `internal/render/common/render-manager.cpp:607` | Upload는 void이며 PreRender 호출부가 실패를 받지 않음. indexed Draw는 graphics object가 null이어도 DrawIndexed까지 진행하는 구조. 실패 전파와 draw 유효성 판단을 함께 검토해야 함 |
| CPU/GPU uniform | `internal/render/renderers/uniform-buffer.cpp:284` | map/lock null은 검사하지만 WriteCPU/WriteGPU는 복사를 건너뛰고 void로 반환. 이는 실패한 uniform의 draw 제외나 host 오류 통지를 보장하지 않음 |

## 검토 요청: 아직 구현하지 않은 선택지

1. **권장: 공개 Graphics API를 유지하면서 Core 내부 실패 전파를 보강한다.** GpuBuffer의 map/lock 실패, vertex/index dirty·element count, geometry draw 유효성, CPU/GPU uniform 실패를 연결한다. 실패한 업로드를 성공 처리하거나 해당 데이터로 draw하지 않도록 하고, callback 미실행과 정상 0-byte 갱신을 구분한다. 재시도와 실행 인스턴스 중단 중 어떤 정책을 적용할지도 구현 전에 확정한다. 관련 native GLES/Vulkan/DYNAMIC 회귀와 map/lock 각각의 실패 주입 검사를 포함한다. 필요하면 내부 함수 반환형·상태를 바꾸지만 공개 Controller/Memory의 시그니처·vtable은 유지하는 범위다.
2. **대안: Web 매핑 실패에서 즉시 실행을 중단한다.** Core를 통과시키지 않는 명시적 fail-fast 정책이다. 예외를 기존 callback state lock 안에서 전파하거나 Wasm abort에 의존하면 정상 해제·비동기 stopped 통지가 보장되지 않는다. 새 Worker/Canvas 종료·재생성 및 host failure 전달을 별도로 설계해야 하므로 자동 채택하지 않았다.

프레임 후 host에서 failure latch만 확인하면 null 역참조보다 늦을 수 있다. 더미 포인터, 무조건 성공, 오류를 숨기는 단순 early return은 선택하지 않는다. 이 두 선택지가 가능한 모든 설계라는 주장은 아니며, 기존 계약 안에서 동등한 안전성을 보이는 대안도 검토할 수 있다.

승인 전에는 Controller factory 분리·WebBufferMemory·shadow 및 후속 wrapper/cache/host 구현을 진행하지 않는다. 이 중단은 웹 메모리 설계 D2의 철회나 전체 요청 범위의 축소가 아니다.

## 검증 결과와 미검증 범위

- 이번 검증: HEAD/status/remote 확인, 실제 소스·주석·호출부 정적 추적, 문서 링크·diff 공백 및 변경 범위·기존 증거 보존 검사. 명령과 개별 exit는 [증거](evidence/p2-b-memory-contract/README.md), 로컬 `logs/p2-b/memory-contract-001/`에 기록한다.
- 생산 코드·빌드 설정 변경 없음. 새 compile/nm/link·CPU/GPU 메모리 단위 검사·GLES/Vulkan/DYNAMIC 회귀는 수행하지 않았다. 중단 결정 전 구현이 없으므로 회귀 성공도 주장하지 않는다.
- 최신 실행 증거는 기존 sync-web-003의 34개 compile/nm 성공과 strict 기본/FULL_ES3 exit 1·10/6개 미해결이다. 이번 결과로 갱신하거나 재실행한 것이 아니다. native build/install/runtime 미검증과 AgingSyncObject 추가 allocation의 성능 미측정도 유지한다.
- WebBufferMemory, 버퍼 소유 shadow, 16-byte/중간 4-byte·callback·offset·uniform·allocation/bounds·double unmap·grow/recycle·종료 검사는 모두 남았다. CPU 검사와 P3 실제 GPU 보존 판정을 분리한다.
- wrapper/capability → shader cache → 실제 main 이벤트/render pthread host → 기본 WebGL2 생산 strict 링크, 정식 source closure/generated source 정리가 남았다. WEB production configure 거부와 P2 미완료를 유지한다.
- 최초 pthread/browser의 로컬 헤더/SW 비교, 별도 승인 실제 Pages 검증, P3/P4 격리 조기 게이트·P4 종료 전 무한 루프 응답성과 전체 pthread/Canvas 종료·재생성 게이트를 유지한다. 배포·서버 구축·SDK 변경 없음.

다음 행동은 **Core 실패 처리 정책과 수정 범위 결정**이다. 승인 후 웹 메모리부터 원래 순서대로 재개한다.
