# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅을 이어가자. 먼저 P1의 미결 결정을 정리해줘.
P1은 설계 초안 작성·결정 대기이며 완료가 아니고 P2는 미착수야.

작업 repo: /home/jy/work/dali/dali-web
기록 기준: dali-web/docs
과거 증거: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web

상위 AGENTS.md, README, docs/STATE.md, HANDOFF.md, PLAN.md,
P1-EXECUTION-CONTRACT.md, evidence/p1/README.md, R2-b-RESULTS.md,
upstreams.lock.json, toolchain.lock.json, BUILDING.md, UPSTREAMS.md를 읽어줘.
실제 HEAD/status/원격을 확인하고 기존 변경과 lock을 보존해줘.
P1 시작은 ef6a1f545ff03df69c3297135048f545ee8dd352였고,
설계 commit은 "Document P1 execution contract and pending decisions"야.

D1 기능/context adapter, D2 shadow 업로드와 partial write 보존,
D3 생산 실행 주체(C pthread 또는 B 단일 Worker), D4 격리 호스팅 허용
각 결정의 권장안과 대안을 짧게 설명해줘. 승인되지 않은 선택을 확정하지 마.
내 결정과 진행 지시를 받은 뒤에만 문서 확정/P2 구현을 진행해줘.

Memory3 WRITE-only는 SDK 6.0.13 guard에서 거부되고,
Memory2도 부분 callback의 기존 byte를 자동 보존하지 않아.
SDK guard 추출 Node 검사는 실제 WebGL runtime 검증이 아니야.
EGL sync/native fence·공유 context 결합의 변경 경계도 확인해줘.
FULL_ES3 링크 지원을 runtime 호환성으로 판단하거나 빈 GL 함수를 만들지 마.
기존 50/100은 mock, GL 두 링크는 exit 1(미해결 7/3)이야.

P2를 승인하면 설계 8절의 최소 생산 GLES 링크 대상과 검증 절차를 따라줘.
생산 소스 source manifest·nm·link map을 남기고 mock 포함을 금지해줘.
실험이 커지거나 합의한 방향을 바꿔야 하면 멈추고 보고해줘.
원본/P0/SDK/R2 증거 보존, 새 실행 디렉터리, 빌드 최대 -j6,
SDK·산출물·프로필·큰 로그 commit 제외 규칙을 지켜줘.
로컬 commit과 대상 원격 push는 허용해. force push 금지,
상위 AGENTS.md의 실제 모델 AI co-author trailer를 정확히 한 번 넣어줘.
Pages 배포와 upstream 갱신은 하지 마.
```
