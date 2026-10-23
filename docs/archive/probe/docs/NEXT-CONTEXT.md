# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅 작업을 이어가자. R2-b만 진행해줘.

작업 repo: /home/jy/work/dali/dali-web
현재 기록 공간: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web

상위 AGENTS.md와 기존 README, docs/STATE.md, HANDOFF.md, PLAN.md,
R2-a-RESULTS.md, TIZEN-BASELINE.md를 먼저 읽어줘.
새 repo README, upstreams.lock.json, toolchain.lock.json,
docs/BUILDING.md, docs/UPSTREAMS.md도 확인해줘.

R2-a commit은 728cf818d4db74890a4c37d01737c8eea3d9a14b야.
실제 HEAD·Git·원격 상태를 재확인하고 기존 변경을 보존해줘.
upstream을 갱신하지 말고 현재 lock을 사용해줘.
새 Core Wasm 빌드·Node 애니메이션 50/100은 통과했지만
새 Adaptor·브라우저·GL 검증은 아직 안 했어.

이번에는 새 경로에서 Firefox 애니메이션, Adaptor 6개 번역 단위,
GL 심볼 링크 탐색을 실행하고 P0 결과와 비교해줘.
독립 R2-b 로그 디렉터리를 사용하고 명령·종료 코드·한계를 기록해줘.
SDK 재사용은 허용하지만 개인 절대 경로를 필수 조건으로 만들지 마.
최대 -j6, SDK·산출물·프로필·대용량 로그는 commit하지 마.
원본 사용자 변경, P0 복제본·로그·SDK와 R2-a 증거는 보존해줘.

R2-b 완료 조건 충족 후 문서 링크를 확인하고 새 dali-web/docs로
기록 기준을 전환해줘. 기존 README·STATE·HANDOFF에 새 경로를 안내하고
P1 인수인계와 다음 컨텍스트 프롬프트를 남겨줘.
생산 렌더러 신규 구현·스레드 설계·Pages 배포는 진행하지 마.
로컬 commit과 대상 원격 push를 허용해. force push하지 말고
모든 생성 commit에 상위 AGENTS.md의 AI co-author trailer 규칙을 지켜줘.
R2-b 완료 후 멈추고 특이 사항·검증 결과·미검증 범위를 보고해줘.
```
