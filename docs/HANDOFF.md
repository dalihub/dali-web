# 다음 context 인수인계: P2-b 환경 분리·생산 GLES 링크

2026-10-23. **P2-a 진단 완료·P2-b 미착수·상위 P2 미완료.** 작업 repo는 `dali-web`, 과거 `webassembly-probe`는 증거/SDK 보존 공간이다.

## 먼저 읽을 것

1. 상위 AGENTS.md, README, [STATE](STATE.md), [PLAN](PLAN.md), [NEXT-CONTEXT](NEXT-CONTEXT.md).
2. [P1 실행 계약](P1-EXECUTION-CONTRACT.md), 특히 D1~D4와 메모리/스레드/호스팅 게이트.
3. [P2-a 결과](P2-a-RESULTS.md), [manifest](P2-a-MANIFEST.md), [P2-b 파일별 순서와 회귀 계획](P2-b-HANDOFF.md), [증거](evidence/p2-a/README.md).
4. upstreams.lock.json, toolchain.lock.json, [BUILDING](BUILDING.md), [UPSTREAMS](UPSTREAMS.md), [MIGRATION](MIGRATION.md).

## 이번 결과와 다음 행동

P2-a 시작 clean main HEAD/원격은 `3eb2c89ecad10f810d2ff064f2b4f3d43a42a426`. 현재 HEAD/status/remote 재확인, 기존 변경 보존. upstream/lock 갱신 금지.

새 `cmake/p2-a/`, `scripts/diagnose-gles.py`, `tests/probes/gles-link-diagnostic.cpp`로 생산 GLES 23개를 포함한 30개 단위 및 Core를 pthread Debug로 컴파일했다. 최종 `diagnostic-003`은 compile/nm 각각 0, strict 기본/FULL_ES3 링크 각각 1·미해결 16/12개다. map은 생성되지 않았다. 실제 Controller/Core 초기화·frame·native sync 경로를 링크 참조하되 함수 실행은 하지 않았다. mock/빈 GL·EGL·sync/undefined 억제 없음.

주요 장애는 EglGraphics RTTI/context 3개, NativeFence/EglSyncObject 3개, program binary utility 5개, GL 5개(FULL_ES3에서는 1개)다. 첫 configure 오류와 초기 harness의 10/6 결과도 보존했다. 자세한 명령·버전·개별 종료 코드는 증거와 로컬 `logs/p2-a/`에 있다.

다음 한 가지 작업은 **P2-b의 공통 GLES/환경별 source 선택과 Controller/context 분리**다. [P2-b 계획](P2-b-HANDOFF.md)의 순서대로 sync → 웹 메모리 → wrapper/capability → shader cache 경계 → 실제 host/strict 링크를 이어간다. 공통 계약 변경이나 예상 밖 플랫폼 전체 포팅이 필요하면 중단·보고한다. 생산 링크 성공까지 P2 미완료이며 P2-b가 커지면 검증 가능한 단위로 중간 인수인계를 남긴다.

## 확정 계약·후속 게이트

1. 공통 GLES와 Native/EGL·Web 구현을 파일 단위로 빌드 선택. Core 계약과 native GLES/Vulkan/DYNAMIC 유지. concrete EGL/FD와 RTTI를 공통 Web 경로에서 제거.
2. 웹 별도 메모리 + 보존이 필요한 버퍼 소유 shadow. native Memory2/3 유지·Memory4 없음. callback/partial/uniform 보존과 map/lock bounds·오류·수명 검증. FULL_ES3·invalidate만으로 해결 처리 금지.
3. main 이벤트 + 별도 pthread Worker update/upload/render. main ready/stop 비동기화·DOM 경계 분리. link-only harness는 실행 host가 아니다.
4. 최초 pthread/browser 단위에서 로컬 헤더/SW 비교 probe 준비, 별도 승인 후 Pages 실제 URL 최초/재방문·업데이트·하위 경로 검증. 실패/과도한 비용 시 직접 호스팅 검토. 다른 관리형 호스팅 금지.
5. P3/P4에서 이벤트 Worker 플레이그라운드 격리 후보 검증, P4 종료 전 무한 루프·전체 pthread/Canvas 종료/재생성 결과 기록. 기본 E는 임의 코드 격리 보장이 아님.
6. P2 strict 생산 링크와 P3 실제 DALi 픽셀/GL 오류/업로드 보존, P4 입력·resize·애니메이션·반복 종료는 별도 게이트. native 회귀도 수행하거나 미검증 범위 명시.

원본 사용자 변경·P0·SDK·P1/R2 증거·lock·기존 Core 산출물 보존. 새 build/log 경로 사용, 최대 -j6, 개인 SDK 경로 필수화 금지. SDK·산출물·프로필·대용량 로그 commit 금지. upstream 갱신·배포·서버 구축 금지. 남긴 서버/브라우저/빌드 프로세스 없음. 이번에는 P2-a 인수인계 후 멈췄다.
