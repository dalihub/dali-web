# R1-a 원본과 subtree 정책 결정

일자: 2026-10-22 (Asia/Seoul). 상태: R1-a 완료. 실제 import·이관·commit·push 없음.

> 후속 변경: 사용자 요청으로 현재 기준은 [Tizen tizen 브랜치](TIZEN-BASELINE.md)로 대체됐습니다. 아래는 R1-a 당시 결정의 이력입니다.

## 고정 원본

| 라이브러리 / prefix | 공개 원본 URL | import 기준 commit | 기준 ref |
|---|---|---|---|
| Core / `libs/dali-core` | https://github.com/dalihub/dali-core.git | `80998009aee9a30ab4c7909c68281ed1983aa4ef` | `refs/tags/dali_2.5.43` |
| Adaptor / `libs/dali-adaptor` | https://github.com/dalihub/dali-adaptor.git | `d76afcfff7a09d788bb0cd3204aace153fce8cfe` | `refs/tags/dali_2.5.43` |
| UI / `libs/dali-ui` | https://github.com/dalihub/dali-ui.git | `13e6bf80ca8cef250a787bb2643899e4d9648cda` | `refs/tags/v2.5.43.11572` |

태그는 annotated tag이므로 위 SHA는 tag object가 아닌 peeled commit이다. 실제 import는 ref 이름만 믿지 않고 이 full SHA와 tree를 확인한다. 원본 전체 tracked tree를 포함하며 foundation·components를 별도 subtree로 나누지 않는다.

Core·Adaptor는 P0 소스와 동일한 공개 릴리스 commit을 선택했다. UI는 P0 검증 대상이 없으므로 같은 2.5.43 계열의 확인된 공개 릴리스 태그를 선택했다. UI 최신 devel의 2.5.44 계열을 섞거나 개인 작업 브랜치를 사용하지 않는다. 같은 버전 계열이라는 사실만으로 세 라이브러리의 빌드 호환성을 보장하지 않으며 UI 연결은 P6에서 검증한다.

## P0 및 로컬 상태와의 비교

| 라이브러리 | P0 commit | 선택 commit의 tree | 비교 결과 |
|---|---|---|---|
| Core | `800960a7e226e1c6afd19b6a492b32ea7c11ed50` | `b78f41485d820f4105a9e2cf07deda6c7bad0326` | P0 tree와 동일, `git diff --exit-code` 0, 파일 변경 없음 |
| Adaptor | `8448c8c07bdec428eee9742eec3db2129a703772` | `5a39f39c8e2533f01f10f19dccc23ff648898dac` | P0 tree와 동일, `git diff --exit-code` 0, 파일 변경 없음 |
| UI | 없음 | `9cc937f5cf4b7a7639fd9a07df39bda532603f77` | P0에 UI 복제본·빌드·실행 없음. 신규 기준 선정 |

P0 Core·Adaptor SHA는 GitHub commit API에서 각각 422였다. 로컬 Tizen master의 merge commit과 공개 릴리스 commit은 이력이 다르다. 선택 commit은 각각 P0 commit의 조상이며 `rev-list --left-right --count P0...selected`는 Core `303 0`, Adaptor `305 0`이다. 이는 파일 변경 수가 아니다. 전체 tree 일치로 tracked 파일 내용·경로·모드가 같음을 확인했다. 생성 헤더·SDK·빌드 산출물은 tree 비교 밖이며 R2에서 다시 생성·검증한다.

원본 Core·Adaptor와 실험 복제본의 `git status --short`는 비어 있다. UI 로컬 HEAD는 `4f9c3f55c31022693ab6de717ee4a9a747a83627`이고 `todo-items/README.md` 수정, `todo-items/recycler-api-abi-refactoring.md` 미추적 파일이 있다. 이 변경은 보존하며 import에 포함하지 않는다. 로컬 HEAD → 선택 UI의 tracked diff는 90 files, 825 insertions, 1261 deletions이고 이력은 `10 28`이다. 이것은 P0 회귀 diff가 아니다.

조회 시 Core master는 `c8900010c9aa1dd1b465e5194ed8acf29820eced`, Adaptor master는 `538b29c6cc0aec49690d1d5cc338527456047b42`, UI devel은 `7dc0ceddf40340695925556ea1a791144f6f2fda`였다. 최신 branch tip은 선정 기준으로 사용하지 않는다. UI의 로컬 origin/devel `f83ddf791351e1e6c385d2098415d612196f7017`도 최신 원격 상태가 아니었다.

## 이력 정책

1. 세 subtree의 최초 add 및 후속 merge/pull에 모두 `--squash` 사용. 전체 원본 이력을 dali-web에 합치지 않는다.
2. 이유: 웹 포팅 commit과 원본 갱신 단위를 읽기 쉽게 분리하고, 세 라이브러리의 과거 이력을 일반 clone의 필수 비용으로 만들지 않는다. 소스 tree·라이선스는 그대로 보존한다.
3. 대가: dali-web만으로 import 이전의 상세 blame·bisect는 불가능하다. 원본 URL·full SHA·tree·태그와 Git subtree 메타데이터를 보존하고 필요할 때 upstream을 fetch하여 조사한다. squash merge commit의 `git-subtree-*` 기록을 임의 삭제하지 않는다.
4. 기각한 대안: non-squash는 전체 이력의 오프라인 조사에 유리하지만 현재 목표인 포팅 변경 추적·일반 clone 재현에 필요하지 않다. Tizen SSH 원본은 공개 재현에 계정 의존성이 있고 GitHub에서 동일 tree를 확보했으므로 선택하지 않는다.
5. 후속 갱신은 한 라이브러리씩 후보 SHA를 명시적으로 검토·fetch하고 `git subtree merge --prefix=libs/<name> <full-sha> --squash`로 적용한다. 충돌 해결과 해당 회귀 검증 후 lock을 갱신한다. 정책을 혼용하거나 branch tip으로 자동 갱신하지 않는다.

## R1-b 생성 대상과 계약

R1-b에서 생성할 파일은 루트 `README.md`, `.gitignore`, `upstreams.lock.json`, `scripts/setup-upstream-remotes.sh`, `docs/UPSTREAMS.md`와 세 `libs/` subtree이다. 그때의 README는 작업 기록 기준이 아직 webassembly-probe임을 안내한다. 실험 코드·toolchain lock·문서 본 이관은 R2, 사이트·서비스·CI 구현은 후속 단계다. 빈 디렉터리 뼈대를 억지로 commit하지 않는다.

lock은 `schemaVersion: 1`, `upstreams` 배열로 구성하고 각 항목에 `name`, `url`, `prefix`, `ref`, `commit`, `tree`, `squash: true`, `remote`, `p0Commit`(UI는 null), `p0TreeEqual`(UI는 null)을 기록한다. 초기 import 완료 후 lock과 subtree tree의 일치를 검사한다. URL·SHA의 기준은 이 문서이며 lock 파일은 이번에 만들지 않았다.

개발자 remote는 `upstream-dali-core`, `upstream-dali-adaptor`, `upstream-dali-ui`를 사용한다. 준비 스크립트는 repo 루트에서 lock을 읽어 없는 remote만 추가하고, 기존 URL이 같으면 유지하며 다르면 실패 후 보고한다. 원격 URL을 조용히 덮어쓰지 않는다. 자동 fetch·pull·push는 하지 않는다. branch 관찰 대상은 Core/Adaptor master, UI devel이지만 import는 위 태그와 SHA로 고정한다. 사용자·CI의 일반 clone 빌드에는 upstream remote가 필요 없다.

subtree는 원본의 전체 tracked tree를 가져온다. 루트 `.gitignore`는 `/tools/`, `/build/`, `/logs/` 등 상위 생성물에 한정한다. 라이브러리의 tracked `build/`에는 빌드 소스가 있으므로 삭제하지 않는다. tracked `*.autogen.*`도 원본 tree 일부이면 유지한다. 중첩 `.git`, 로컬 SDK·프로필·산출물을 파일 복사로 섞지 않는다. 원본 LICENSE 계열과 third-party 고지를 모두 보존하고 R1-b에서 실제 목록을 확인한다.

R1-b는 subtree 명령 자체가 commit을 생성한다. 다음 실행 요청에 로컬 commit 허용 범위가 있어야 하며, commit 금지가 유지되면 import 전에 멈춘다. push는 별도 실행 범위다. 다음 세션의 실제 모델명으로 상위 AGENTS의 AI co-author trailer 규칙을 모든 생성 commit에 적용·검증한다.

## 검증 증거와 한계

- 로컬 SHA·tree·diff·상태 (로컬 보존 자료: `webassembly-probe/logs/r1-a/local-verification.json`; [위치 안내](../../../MIGRATION.md)): tree·diff·ancestor 검사 종료 코드 모두 0.
- 선택 UI (로컬 보존 자료: `webassembly-probe/logs/r1-a/ui-selected-verification.json`; [위치 안내](../../../MIGRATION.md)), 최신 UI 비교 조사 (로컬 보존 자료: `webassembly-probe/logs/r1-a/ui-verification.json`; [위치 안내](../../../MIGRATION.md)).
- 원격 refs: Core (로컬 보존 자료: `webassembly-probe/logs/r1-a/dali-core-refs.txt`; [위치 안내](../../../MIGRATION.md)), Adaptor (로컬 보존 자료: `webassembly-probe/logs/r1-a/dali-adaptor-refs.txt`; [위치 안내](../../../MIGRATION.md)), UI (로컬 보존 자료: `webassembly-probe/logs/r1-a/dali-ui-refs.txt`; [위치 안내](../../../MIGRATION.md)), 대상 (로컬 보존 자료: `webassembly-probe/logs/r1-a/dali-web-refs.txt`; [위치 안내](../../../MIGRATION.md)). 네 조회 모두 종료 0, 대상은 refs 출력 없음.
- API 및 환경 확인 기록 (로컬 보존 자료: `webassembly-probe/logs/r1-a/remote-checks.txt`; [위치 안내](../../../MIGRATION.md)). 대상은 public이며 `/home/jy/work/dali/dali-web`은 아직 없다.
- 이력 동작은 설치된 Git 2.34.1의 `/usr/share/doc/git/contrib/subtree/git-subtree.txt` add/merge/--squash 설명 확인. 실제 subtree 실행은 하지 않았다.
- 빌드·Node·Firefox는 재실행하지 않았다. P0 결과는 기존 검증 범위에만 유효하며 R2 재현·P6 UI 호환성 검증을 대체하지 않는다.
