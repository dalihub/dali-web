# DALi Web

DALi의 브라우저 포팅 저장소입니다. Core·Adaptor·UI 소스와 실제 생산 GLES 렌더러·pthread host의 [기본 WebGL2 빌드](docs/BUILDING.md)를 제공합니다. 생산 strict 링크와 [최초 실제 브라우저 렌더링](docs/P3-INITIAL-RESULTS.md)을 검증했습니다. 전체 GPU 메모리·격리·종료/재생성 인수는 진행 중입니다.

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

[P2-b 생산 구현과 P2 링크](docs/P2-b-PRODUCTION-RESULTS.md)를 완료했습니다. 실제 Buffer/Controller/Core uniform CPU 18모드와 wrapper CPU 5모드를 검사했고, main 이벤트·pthread 렌더 host의 비동기 failure/stop을 구현했습니다. 실제 두 프레임 픽셀과 정상 stopped 통지는 검증했으며, GPU 부분 갱신·수명·전체 종료/재생성은 P3/P4에 남습니다.

R2-b 재검증과 기록 전환을 완료했고, [P1 실행 계약](docs/P1-EXECUTION-CONTRACT.md)을 작성했습니다. D1~D4 사용자 결정을 반영해 P1 설계를 완료했습니다. GitHub Pages 우선 검증, 실패 시 직접 호스팅 검토이며 실제 호스팅·생산 실행은 미검증입니다. 현재 시작점은 [STATE](docs/STATE.md), [설계 결정·P2 인수인계](docs/HANDOFF.md), [PLAN](docs/PLAN.md), [JOURNAL](docs/JOURNAL.md)입니다. 다음 요청문은 [NEXT-CONTEXT](docs/NEXT-CONTEXT.md)에 있습니다.

과거 R2-b에서는 Core의 Wasm·Node·Firefox 모의 애니메이션 50/100과 Adaptor 6개 파일 컴파일을 검증했습니다. GL 탐색은 265개 심볼 중 기본 7개/FULL_ES3 3개 미해결이며 당시 생산 링크·픽셀 렌더링은 미완료였습니다. 최신 생산 링크 결과는 위 P2-b 문서를 따릅니다. [R2-b 결과](docs/R2-b-RESULTS.md), [빌드·재현](docs/BUILDING.md).

이전 `webassembly-probe`는 과거 증거·SDK·P0 복제본 보존 공간입니다. [이관 기록](docs/MIGRATION.md)과 P2-a/sync 중간 결과는 역사 자료이며 현재 생산 링크 결과와 구분합니다.

2026-10-26 [서버 없는 플레이그라운드 컴파일 방향](docs/PLAYGROUND-COMPILATION.md)을 채택했습니다. 원본 Emception 3.1.24와 같은 SDK 계열로 미리 빌드한 DALi를 정적으로 제공하고 사용자 C++를 브라우저에서 컴파일·링크합니다. [실제 생산 링크와 사용자 함수 실행 검사](docs/EMCEPTION-3.1.24-ALIGNMENT.md)는 통과했습니다. 기존 6.0.13 lock은 과거 재현용으로 보존하며 다음 작업은 3.1.24 기준 P3 실제 DALi 렌더링·GPU 검증입니다. 플레이그라운드 서비스·실제 Pages·전체 종료 검증은 아직 완료하지 않았습니다.
