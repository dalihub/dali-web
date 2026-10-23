# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅을 이어가자. P1의 D1·D2 확정 내용을 읽고 미결 항목 D3부터 논의를 이어가자.
D1 아키텍처와 D2 업로드 설계는 승인됐고 D3~D4가 미결이어서 P1은 미완료, P2는 미착수야.

작업 repo: /home/jy/work/dali/dali-web
기록 기준: dali-web/docs
과거 증거: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web

상위 AGENTS.md, README, docs/STATE.md, HANDOFF.md, PLAN.md,
P1-EXECUTION-CONTRACT.md, evidence/p1/README.md, R2-b-RESULTS.md,
upstreams.lock.json, toolchain.lock.json, BUILDING.md, UPSTREAMS.md를 읽어줘.
실제 HEAD/status/원격을 확인하고 기존 변경과 lock을 보존해줘.
P1 시작은 ef6a1f545ff03df69c3297135048f545ee8dd352였고,
최초 설계 commit은 d2964ae0910cc475992ed1c396247db150791b68이고,
D1 후속은 "Record approved GLES platform separation design"이야.

D1 확정: 공통 GLES 로직과 Native/EGL·Web 환경 구현을 파일 단위로 분리하고
빌드 시 선택해줘. Core 계약과 기존 GLES/Vulkan/DYNAMIC을 유지하고,
컨텍스트·표면 / 업로드 / sync·native interop의 책임을 분리하는 정식 포팅 방향이야.
EGL concrete cast·타입은 native 구현에 두고 웹 미지원 기능을 빈 함수로 만들지 마.
설계 3절의 D1 계약을 따라줘. 구현은 아직 시작하지 마.

D2 확정: 웹 전용 메모리 구현과 보존이 필요한 버퍼 소유 CPU shadow를 사용해줘.
매핑 객체는 접근·업로드를 담당하고 native Memory2/3는 유지해줘. Memory4는 추가하지 마.
명시적 부분 갱신과 callback을 구분하고 저장소 수명·보존 의미·비용을 검증해야 해.
D2 후속 commit 제목은 "Record approved web buffer upload design"이야.
먼저 D3의 실행 스레드 구조를 설명해줘. D3/D4는 미결이며 D1·D2 승인을 확대 해석하지 마.
남은 논의를 끝내고 명시적인 구현 지시를 받은 뒤에 P2를 시작해줘.

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
