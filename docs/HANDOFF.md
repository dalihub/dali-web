# 다음 context 인수인계: P2-b 웹 메모리부터

2026-10-23. **P2-b sync 중간 구현, 전체 P2-b·상위 P2 미완료.** 작업 repo `dali-web`, 기록 `dali-web/docs`; 과거 `webassembly-probe`는 SDK/증거 보존 공간.

## 먼저 읽을 것

상위 AGENTS.md, README, [STATE](STATE.md), [PLAN](PLAN.md), [NEXT-CONTEXT](NEXT-CONTEXT.md), [P1 계약](P1-EXECUTION-CONTRACT.md), [P2-a manifest](P2-a-MANIFEST.md)·[결과](P2-a-RESULTS.md), [context 결과](P2-b-RESULTS.md), [sync 결과](P2-b-SYNC-RESULTS.md), [전체 순서](P2-b-HANDOFF.md), [sync 증거](evidence/p2-b-sync/README.md). upstreams/toolchain lock, BUILDING·UPSTREAMS·MIGRATION도 따른다.

이번 시작 clean main HEAD는 `7b6ee23ff36b3658fc957e1607de2d21963796c1`, 조회한 원격 main은 `a68e3f00f0c7752561137c179500b004b930285e`. 로컬 checkpoint는 실제 git log/status로 확인한다. push 없음. 원본/SDK/과거 산출물/lock 변경 금지.

## 구현·검증과 한계

1. Controller/context/native rendering 분리와 Web 단일 cache/identity에 이어 sync를 파일 선택으로 분리했다. 공통 SyncPool header의 concrete EGL 저장소도 cpp 내부로 이동했다. native policy/FD/age 동작 유지, Core 공통 계약 변경 없음.
2. WebFence는 pending/complete/failed/cancelled 상태와 실제 zero-timeout GL poll, owner 삭제, 다른 스레드 해제 요청, 종료 cancellation을 구현한다. frame age·flush는 완료 근거가 아니다. Web shared fence는 frame 내 재사용하지 않는다. native FD/EGL/cross-context GPU wait는 거부한다.
3. 실제 host는 아직 없다. host는 GL owner에서 `WebFence::HasFailure()`를 확인해 비동기 failure/stop으로 전달해야 한다. 이 latch는 thread 수명 동안 유지하며 실패 후 새 Worker/인스턴스를 사용한다. 마지막 제출·회수 후 fence shutdown → context 폐기 순서를 지킨다. Core bool 완료 계약을 실패로 확대하지 않는다.
4. 최종 `logs/p2-b/sync-web-003`: Core 333단계, 34개 compile/nm 각각 0. 기본/FULL_ES3 strict 각각 exit 1·10/6개 미해결, map 없음. NativeFence 미해결 3개 제거. cache utility 5개와 wrapper GL 5/1개가 남는다. `fence-unit-002`는 GL 응답 주입형 Node 수명 검사이며 실제 browser/GPU 증거가 아니다.
5. native source 회귀 통과. `sync-native-002`의 세 native configure는 host compiler 부재로 실패. native context/rendering/sync/sync-pool/egl-graphics 5개 Emscripten 교차 compile/nm 통과는 native build/runtime 증거가 아니다.
6. WEB production configure 거부 유지. `cmake/p2-b`는 Web sync 선택 후에도 native Memory2/3·wrapper·cache가 남는 부분 진단이다. 정식 source closure와 native generated source 전체 정리는 남았다. 과거 `cmake/p2-a`는 native 추출을 반영했으며 과거 정확한 재현은 해당 commit 기준이다.

## 다음 한 가지 행동

[P2-b 순서](P2-b-HANDOFF.md) 4번 웹 메모리. Controller MapBufferRange factory를 환경별로 선택하고 WebBufferMemory와 필요한 버퍼 소유 CPU shadow를 구현한다. native Memory2/3 유지, Memory4 없음. callback 0/부분/전체, nonzero offset, map/lock 범위와 overflow, allocation 실패, double unmap, grow/recycle·종료 수명을 확인한다. Core 호출부의 실패 처리에 공통 계약 변경이 필요하면 먼저 보고한다.

이후 wrapper/capability → source shader/cache 경계 → 실제 main 이벤트 + render pthread host → 기본 WebGL2 생산 strict 링크와 manifest/nm/map이다. unused archive·빈 GL/EGL/sync·undefined 억제로 성공 처리하지 않는다. 이 단계들은 아직 실행 완료하지 않았고 다음 범위로 남는다.

## 보존·후속 게이트

- 공통 계약 변경·전체 플랫폼 포팅·실험 확대·합의 방향 변경이 필요하면 중단·보고. native 회귀는 수행하거나 미검증 이유/영향 명시.
- 최초 pthread/browser 단위에 로컬 헤더/SW 비교 probe 준비. 별도 승인 후 Pages 실제 최초/재방문·업데이트/해제·하위 경로 검증. 실패/과도한 비용이면 직접 호스팅 검토, 다른 관리형 서비스 금지.
- P3 실제 DALi 픽셀/GL 오류/업로드 보존은 별도. P3/P4 이벤트 Worker 격리 후보 조기 검증, P4 종료 전 무한 루프 중 응답성·전체 pthread/Canvas 종료·재생성 기록.
- 원본/P0·SDK·R2/P1/P2-a/context 증거·기존 build/log 보존, 최대 -j6. upstream 갱신·Pages 배포·서버 구축 금지. 개인 SDK 경로 필수화·대형 산출물 commit 금지.

현재 미결 설계 결정 없이 웹 메모리부터 진행 가능하다. sync 중간 인수인계를 전체 P2-b 또는 상위 P2 완료로 간주하지 않는다.
