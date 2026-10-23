# DALi Web

DALi의 브라우저 포팅 저장소입니다. Core·Adaptor·UI 소스를 포함하며 모의 그래픽 기반 [Core Wasm 빌드·Node 검사](docs/BUILDING.md)를 제공합니다. 실제 Canvas 렌더링은 아직 제공하지 않습니다.

## 소스 받기

```sh
git clone https://github.com/dalihub/dali-web.git
cd dali-web
```

일반 clone에 세 라이브러리의 전체 tracked 소스가 포함됩니다. submodule 초기화는 필요하지 않습니다. SDK·빌드 결과·로그는 포함하지 않습니다.

## Upstream 관리

현재 기준은 세 원본의 [Tizen tizen 브랜치](docs/TIZEN-BASELINE.md)입니다. 고정 URL·full commit SHA·tree·ref는 [upstreams.lock.json](upstreams.lock.json), 갱신 정책과 라이선스는 [UPSTREAMS](docs/UPSTREAMS.md)에 있습니다. 개발자 remote가 필요하면 Git과 Python 3 환경에서 실행합니다.

```sh
./scripts/setup-upstream-remotes.sh
```

이 스크립트는 fetch·pull·push하지 않습니다. 일반 clone 사용에 upstream remote는 필요하지 않습니다.

## 진행 기록

R2-b 재검증과 기록 전환을 완료했고, [P1 실행 계약 초안](docs/P1-EXECUTION-CONTRACT.md)을 작성했습니다. P1은 사용자 결정 대기이며 완료로 선언하지 않았습니다. 현재 시작점은 [STATE](docs/STATE.md), [설계 결정·P2 인수인계](docs/HANDOFF.md), [PLAN](docs/PLAN.md), [JOURNAL](docs/JOURNAL.md)입니다. 다음 요청문은 [NEXT-CONTEXT](docs/NEXT-CONTEXT.md)에 있습니다.

현재 Core의 Wasm·Node·Firefox 모의 애니메이션 50/100과 Adaptor 6개 파일 컴파일을 검증했습니다. GL 탐색은 265개 심볼 중 기본 7개/FULL_ES3 3개 미해결이며 실제 생산 링크·픽셀 렌더링은 미완료입니다. [R2-b 결과](docs/R2-b-RESULTS.md), [빌드·재현](docs/BUILDING.md).

이전 `webassembly-probe`는 과거 증거·SDK·P0 복제본 보존 공간입니다. 문서 사본과 미이관 자료 위치는 [MIGRATION](docs/MIGRATION.md)에 있습니다. 계획은 자동 실행 지시가 아니며 P1의 D1~D4 결정 후 P2 진행 여부를 판단합니다. P2 구현은 시작하지 않았습니다.
