# DALi 브라우저 포팅 작업 공간

DALi C++ 코드를 WebAssembly로 실행하고 기존 GLES 렌더러를 통해 브라우저 Canvas에 표시하기 위한 실험 공간입니다. 현재 Core의 모의 그래픽 기반 애니메이션 실행만 검증됐으며, 실제 DALi 픽셀 렌더링은 미완료입니다.

## 새 context에서 시작하기

1. 상위 AGENTS.md (로컬 보존 자료: `AGENTS.md`; [위치 안내](../../MIGRATION.md))의 지침을 읽습니다.
2. [현재 상태](docs/STATE.md)와 [현재 인수인계](docs/HANDOFF.md)를 읽습니다.
3. [작업 계획](docs/PLAN.md)에서 이번에 진행할 단계만 읽습니다.
4. 필요한 소스와 로그만 확인하고, 사용자가 지정한 범위에서 진행합니다.

R1-a·R1-b 및 상위 R1은 완료했습니다. 새 repo `/home/jy/work/dali/dali-web`의 `main`에 세 subtree와 관리 뼈대를 commit·push했습니다. 현재 소스 기준은 [Tizen tizen 브랜치](docs/TIZEN-BASELINE.md)로 갱신했습니다. [R1-b 결과](docs/R1-b-RESULTS.md)와 [현재 R2-b 인수인계](docs/HANDOFF.md)를 참고합니다. [R2-a](docs/R2-a-RESULTS.md)의 빌드 이관·현재 Core Node 검증도 완료했습니다. 이후 순서는 R2-b 브라우저 검증·기록 전환 → P1 실행 계약 설계입니다. 계획 항목은 자동 실행 지시나 사용자가 명시적으로 추가한 작업큐가 아닙니다.

## 기록 위치

| 파일 | 역할 |
|---|---|
| [PLAN.md](docs/PLAN.md) | 단계, 의존 관계, 완료 조건, context 분할 기준 |
| [STATE.md](docs/STATE.md) | 현재 상태의 기준 문서, 검증 범위, 미결 사항 |
| [HANDOFF.md](docs/HANDOFF.md) | 다음 context의 목표와 첫 행동 |
| [JOURNAL.md](docs/JOURNAL.md) | 작업별 변경과 검증 이력 |
| [handoffs](docs/handoffs) | 교체 전 인수인계 문서 보관 |
| [RESULTS.txt](RESULTS.txt) | 최초 기술 검증 결과 스냅샷 |
| logs/ | 빌드·실행 증거 |

## 기록 갱신 방법

1. 시작할 때 실제 Git 상태와 소스 revision을 확인합니다. STATE에 적힌 과거 상태를 현재 사실로 가정하지 않습니다.
2. 한 번에 하나의 검증 가능한 목표를 선택합니다. 진행 중인 단계는 STATE에 표시합니다.
3. 설계 선택에는 선택 이유와 기각한 대안 중 중요한 것만 JOURNAL에 기록합니다. 제안과 합의된 결정을 구분합니다.
4. 완료·실패·보류 시 명령, 종료 코드, 증거 경로, 검증 한계를 기록합니다. 다음 실험 전 기존 로그를 별도 실행 디렉터리에 보존합니다.
5. context를 옮길 때 기존 HANDOFF를 `docs/handoffs/<작업번호>-<단계>.md`로 보관하고 HANDOFF를 현재 상태로 교체합니다. STATE와 PLAN의 상태도 함께 맞춥니다.
6. 미완료 코드, 실행 중 프로세스, 생성 파일, 원본 저장소와 실험 복제본의 차이를 남깁니다. 새 context가 실패한 접근을 반복하지 않도록 실패 원인과 다음 한 가지 실험을 적습니다.

JOURNAL은 이력을 추가하고, STATE와 HANDOFF는 최신 내용으로 유지합니다. 상태가 충돌하면 실제 파일·revision·로그를 확인한 뒤 바로잡습니다. 날짜는 Asia/Seoul 기준입니다.

## 저장소와 도구

`src/dali-core`, `src/dali-adaptor`는 원본 저장소의 격리된 로컬 복제본입니다. `tools/`는 로컬 SDK와 빌드 도구이고, `build/`와 `logs/`는 생성물입니다. 이 상위 작업 공간 자체는 현재 Git 저장소로 인식되지 않으므로 문서가 자동으로 버전 관리되지는 않습니다. [dalihub/dali-web](https://github.com/dalihub/dali-web)에 R1-b 소스·lock·관리 문서를 commit·push했습니다. 이 기존 작업 공간의 최신 기록은 아직 로컬 문서입니다.

R1에서 단일 dali-web repo의 subtree 구성을 완료했고, R2-a에서 실험 코드·빌드 경로를 이관했습니다. R2-b에서 브라우저·GL 재검증과 기록 전환을 진행할 계획입니다. 이관 완료 전에는 이 디렉터리를 기록 기준으로 유지합니다. 복제본 diff와 최초 검증 증거를 먼저 보존하고 검토합니다. SDK, 빌드 결과, 브라우저 프로필을 소스 변경과 섞어 커밋하지 않습니다.
