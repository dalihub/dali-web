# DALi Web

DALi의 브라우저 포팅 저장소입니다. R1-b에서 Core·Adaptor·UI 소스와 upstream 관리 뼈대를 구성합니다. 실제 Canvas 렌더링과 웹 빌드 절차는 아직 제공하지 않습니다.

## 소스 받기

```sh
git clone https://github.com/dalihub/dali-web.git
cd dali-web
```

일반 clone에 세 라이브러리의 전체 tracked 소스가 포함됩니다. submodule 초기화는 필요하지 않습니다. SDK·빌드 결과·로그는 포함하지 않습니다.

## Upstream 관리

고정 URL·full commit SHA·tree·ref는 [upstreams.lock.json](upstreams.lock.json), 갱신 정책과 라이선스는 [UPSTREAMS](docs/UPSTREAMS.md)에 있습니다. 개발자 remote가 필요하면 Git과 Python 3 환경에서 실행합니다.

```sh
./scripts/setup-upstream-remotes.sh
```

이 스크립트는 fetch·pull·push하지 않습니다. 일반 clone 사용에 upstream remote는 필요하지 않습니다.

## 진행 기록

R2-b 완료 전 기록 기준은 기존 작업 공간 `webassembly-probe/docs`입니다. 이 repo의 [R1-a 결정](docs/r1-a/R1-a-UPSTREAMS.md)과 [R1-b 시작 인수인계](docs/r1-a/HANDOFF.md)는 변경하지 않는 역사적 스냅샷입니다. 원문에 있는 이전 공간의 증거 링크는 일반 clone에 포함되지 않으며, 위치 매핑은 [스냅샷 안내](docs/r1-a/README.md)를 참고합니다.

다음 단계 R2-a는 기존 실험 코드의 경로 이관과 Core 빌드·Node 검사입니다. R2-b에서 브라우저·GL 탐색을 재검증한 뒤 기록 기준을 전환합니다.
