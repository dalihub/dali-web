# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅을 이어가자. P2-b 공통 GLES/Native-EGL/Web 분리와 생산 링크 구현을 진행해줘.
P1 D1~D4는 확정됐고 P2-a 진단은 완료됐어. 상위 P2는 아직 미완료야.

작업 repo: /home/jy/work/dali/dali-web
현재 기록: dali-web/docs
과거 증거·SDK 보존 공간: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web
P1 완료 commit: 3eb2c89ecad10f810d2ff064f2b4f3d43a42a426

상위 AGENTS.md, README, docs/STATE.md, HANDOFF.md, PLAN.md,
P1-EXECUTION-CONTRACT.md, P2-a-MANIFEST.md, P2-a-RESULTS.md,
P2-b-HANDOFF.md, evidence/p2-a/README.md, NEXT-CONTEXT.md,
upstreams.lock.json, toolchain.lock.json, BUILDING.md, UPSTREAMS.md,
MIGRATION.md를 먼저 읽어줘. 실제 HEAD/status/원격을 확인하고 기존 변경·lock을 보존해줘.

P2-a 결과:
- 별도 pthread Core Debug build 333단계 통과.
- 생산 GLES 23개 포함 30개 번역 단위 compile/nm 각각 exit 0.
- 최종 strict WebGL2/FULL_ES3 링크 각각 exit 1, 미해결 16/12개. map 미생성.
- EglGraphics RTTI/context 3, native fence 3, program cache utility 5, GL 5/1개.
- logs/p2-a/diagnostic-003이 최종 결과. 001 configure 실패와 002 초기 harness 결과도 보존.
- cmake/p2-a, scripts/diagnose-gles.py, tests/probes/gles-link-diagnostic.cpp는 링크 전용.
  실제 Controller/Core·phase2·frame·native sync를 참조하지만 함수 실행/host 구현은 없어.
- 초기 harness는 GC로 native 오류가 가려져 10/6개였고, 참조 보강 후 16/12개였어.
  unused archive/불충분한 도달 경로의 링크 성공을 생산 성공으로 선언하지 마.

확정 계약:
1. 공통 GLES와 Native/EGL·Web 환경 구현을 파일 단위로 분리하고 빌드 시 선택.
   Core 계약과 기존 GLES/Vulkan/DYNAMIC 유지, concrete EGL/native FD/RTTI는 native에 격리.
2. 웹 별도 메모리 구현 + 보존이 필요한 버퍼 소유 CPU shadow.
   native Memory2/3 유지, Memory4 추가 없음. callback/partial/uniform 보존과 오류/수명 검증.
3. main = DALi 이벤트, 별도 pthread Worker = 업데이트·업로드·렌더.
   DOM/이벤트 경계 분리, main ready/stop blocking 제거. 이벤트 Worker 지원은 미확정 후보.
4. GitHub Pages + SW 격리 우선 검증, 실패/과도한 비용 시 직접 호스팅 검토.
   다른 관리형 호스팅은 금지. D3 임의 변경 금지. 이번 배포·서버 구축 금지.

P2-b-HANDOFF.md의 변경 파일·선행 관계·회귀 계획대로 진행해줘.
첫 작업은 source 선택과 Controller/context 분리야.
sync → 웹 메모리 → GL wrapper/capability → shader cache 경계 → 실제 host 순서로 이어가되,
구현 단위가 커지면 검증 가능한 지점에서 중간 인수인계를 남겨줘.
Core/GraphicsInterface/GraphicsSyncAbstraction 공통 계약 변경이 필요하거나
전체 플랫폼 포팅으로 커지거나 합의 방향을 바꿔야 하면 먼저 멈추고 보고해줘.

mock Controller, 빈 GL/EGL/sync, undefined-symbol 억제로 성공 처리 금지.
FULL_ES3의 링크 지원을 runtime/데이터 보존으로 해석하지 마.
실제 생산 Controller/Core·자원·명령·host가 포함된 기본 WebGL2 strict 링크 exit 0,
manifest/nm/map과 개별 명령·버전·종료 코드가 필요해.
생산 링크 성공 전 상위 P2 완료 선언 금지, 실제 픽셀은 P3 별도 게이트야.
변경한 공통/native 경로는 GLES/Vulkan/DYNAMIC 회귀를 수행하거나
미검증 이유·영향 범위를 보고해줘.

최초 pthread/browser 단위에는 로컬 헤더/SW 비교 probe 준비,
별도 승인 후 실제 Pages URL의 최초/재방문·업데이트·하위 경로 검증을 유지해줘.
P3/P4에서 플레이그라운드 격리 후보를 조기 검증하고 P4 종료 전
무한 루프 중 응답성과 전체 pthread/Canvas 종료·재생성 결과를 기록해줘.

원본 사용자 변경·P0·SDK·R2/P1/P2-a 증거·기존 산출물 보존.
upstream 갱신 금지, 별도 build/log, 최대 -j6, 개인 SDK 절대 경로 필수화 금지.
SDK·산출물·프로필·대용량 로그 commit 금지.
로컬 commit·대상 원격 push 허용, force push 금지.
생성 commit마다 상위 AGENTS.md의 실제 모델 AI co-author trailer를 정확히 한 번 넣어줘.
PLAN·STATE·JOURNAL·HANDOFF·다음 프롬프트를 갱신하고
특이 사항·미검증 범위·다음 진행 가능 여부를 보고해줘.
```
