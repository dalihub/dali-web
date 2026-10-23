# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅을 이어가자. P2-a 최소 생산 GLES 링크 대상·의존성 진단을 진행해줘.
P1 설계는 D1~D4 사용자 결정 반영으로 완료됐고, P2는 미착수야.

작업 repo: /home/jy/work/dali/dali-web
현재 기록: dali-web/docs
과거 증거 보존 공간: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web

상위 AGENTS.md, README, docs/STATE.md, HANDOFF.md, PLAN.md,
P1-EXECUTION-CONTRACT.md(특히 3~8절), NEXT-CONTEXT.md,
R2-b-RESULTS.md, evidence/p1/README.md, MIGRATION.md,
upstreams.lock.json, toolchain.lock.json, BUILDING.md, UPSTREAMS.md를 먼저 읽어줘.
P1 마감 commit 제목은 "Finalize P1 hosting policy and P2 handoff"야.
실제 HEAD/status/원격을 확인하고 기존 변경을 보존해줘. upstream/lock은 갱신하지 마.

확정 계약:
1. 공통 GLES와 Native/EGL·Web 환경 구현을 파일 단위로 분리하고 빌드 시 선택.
   Core 계약과 기존 GLES/Vulkan/DYNAMIC 유지, concrete EGL/native FD 결합은 native에 격리.
2. 웹 전용 메모리 구현 + 보존이 필요한 버퍼 소유 CPU shadow.
   매핑 객체는 접근/업로드 담당, native Memory2/3 유지, Memory4 추가 없음.
3. 브라우저 main = DALi 이벤트, 별도 pthread Worker = 업데이트·업로드·렌더.
   DOM/이벤트 경계 분리, main 초기화·종료 blocking 제거.
   이벤트 Worker는 플레이그라운드 격리 후보이며 지원 확정 아님.
4. GitHub Pages + SW 격리 우선 검증, 실패/과도한 비용 시 직접 호스팅 전환 검토.
   다른 관리형 호스팅 서비스는 사용 불가. D3를 호스팅 때문에 임의 변경하지 마.
   Pages 배포·서버 구축은 별도 승인이 필요해.

이번 P2-a 범위:
- 설계 8절을 근거로 명시적 생산 GLES source manifest와 의존성을 작성해줘.
- 별도 build/log 경로에서 필요한 작은 CMake·진단 harness를 준비하고,
  현재 소스의 컴파일·strict 링크 진단을 수행해줘.
- nm/linker 진단으로 Core/utility/GL/EGL/native 및 concrete 타입 의존성을 분류하고,
  실제 사용 파일·명령·버전·개별 종료 코드·검증 한계를 남겨줘.
- mock Controller, 빈 GL/EGL/sync 함수, undefined-symbol 억제로 성공시키지 마.
  unused archive만 연결해 생산 링크 성공으로 판정하지 마.
- 광범위한 Controller 분리·sync·shadow 구현은 P2-b로 넘기고,
  진단 근거로 변경 파일·적용 순서·회귀 검증 계획을 구체화해줘.
- 진단 완료와 P2-b 인수인계를 남기고 멈춰줘.
  생산 링크 성공 전에는 상위 P2 완료를 선언하지 마.

현재 검증 사실:
Core Wasm·Node·Firefox 50/100은 mock 검사이며 Adaptor 6개 파일 컴파일만 통과했어.
GL 탐색 Python 0, 실제 두 링크는 각각 1, 미해결 7개/3개야.
Memory3 WRITE-only는 SDK guard에서 거부되고 Memory2도 부분 쓰기를 자동 보존하지 않아.
FULL_ES3 링크 지원과 실제 런타임 호환성·데이터 보존을 구별해줘.
생산 픽셀·전체 Adaptor·pthread·OffscreenCanvas·Pages는 미검증이야.

후속 게이트도 보존해줘:
- 최초 pthread/browser 검증 단위에서 로컬 헤더/SW 비교 probe 준비,
  별도 승인 후 실제 Pages URL의 최초/재방문·업데이트·하위 경로 검증.
- P2 이후 P3/P4 구간에 플레이그라운드 격리 조기 검증,
  P4 종료 전 가능성·장애·변경 범위 기록. 무한 루프와 전체 실행 종료/재생성 포함.

원본 사용자 변경·P0·SDK·R2/P1 증거를 보존해줘.
SDK 재사용 가능, 개인 절대 경로를 필수 조건으로 만들지 말고 빌드는 최대 -j6.
SDK·산출물·프로필·대용량 로그는 commit하지 마.
로컬 commit과 대상 원격 push는 허용해. force push 금지,
모든 생성 commit에 상위 AGENTS.md의 실제 모델 AI co-author trailer를 정확히 한 번 넣어줘.
실험이 커지거나 합의한 방향을 바꿔야 하면 멈추고 보고해줘.
PLAN·STATE·JOURNAL·HANDOFF·다음 프롬프트를 갱신하고
설계 대비 특이 사항·미검증 범위·P2-b 진행 가능 여부를 보고해줘.
```
