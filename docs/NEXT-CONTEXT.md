# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅 작업을 이어가자. P1 실행 계약 설계만 진행해줘.

작업 repo: /home/jy/work/dali/dali-web
현재 기록 기준: /home/jy/work/dali/dali-web/docs
과거 증거 보존 공간: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web

상위 AGENTS.md, repo README, docs/STATE.md, HANDOFF.md, PLAN.md의 P1,
R2-b-RESULTS.md, MIGRATION.md, TIZEN-BASELINE.md,
upstreams.lock.json, toolchain.lock.json, docs/BUILDING.md, UPSTREAMS.md를 먼저 읽어줘.
실제 HEAD·Git·원격 상태를 재확인하고 현재 lock을 유지해줘.
R2-b commit은 Git log의 "Validate R2-b probes and migrate project records"로 확인해줘.

R2-a Core Wasm·Node와 R2-b Firefox 모의 애니메이션 50/100,
현재 Adaptor 6개 번역 단위 컴파일을 통과했어.
GL 탐색 Python 종료 0이지만 두 링크는 각각 종료 1이야.
265개 심볼 중 기본 7개/FULL_ES3 3개 미해결로 P0와 동일해.
실제 DALi 픽셀 렌더링 성공으로 해석하지 마.

현재 소스의 Core 초기화·GLES Controller·컨텍스트/표면·프레임·
리소스 업로드·해제 호출 순서와 파일 경계를 근거로 정리해줘.
스레드 후보와 Pages 호스팅 조건의 양립 여부, 미지원 기능 정책,
버퍼 업로드 의미와 검증 방법, P2 최소 링크 범위·의존성을 설계해줘.
기존 모의 테스트의 단일 스레드를 생산 설계로 미리 확정하지 마.
생산 렌더러 구현·Pages 배포·upstream 갱신은 하지 마.

원본 사용자 변경·P0 복제본·SDK·R2-a/R2-b 증거를 보존해줘.
필요한 실험은 별도 P1 실행 디렉터리에 명령·버전·종료 코드·한계를 남겨줘.
SDK 재사용 가능, 개인 절대 경로를 필수 조건으로 만들지 말고 빌드는 최대 -j6.
SDK·산출물·프로필·대용량 로그는 commit하지 마.
로컬 commit과 대상 원격 push를 허용해. force push하지 말고
생성 commit에 상위 AGENTS.md의 AI co-author trailer 규칙을 지켜줘.
P1 설계 결과·미결 결정·미검증 가정과 P2 인수인계를 남기고 멈춰줘.
```
