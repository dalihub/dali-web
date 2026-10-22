# 다음 context 인수인계

갱신일: 2026-10-22. P0·R0·R1-a 완료. 다음 작업은 R1-b repo 뼈대·subtree 구성입니다.

## 먼저 읽을 것

1. `/home/jy/work/dali/AGENTS.md`.
2. README·STATE·PLAN의 R1-b 부분.
3. [R1-a 결정](R1-a-UPSTREAMS.md): 원본·full SHA·tree·squash 정책·lock 계약·생성 파일 목록과 증거.

## 다음 첫 행동과 범위

먼저 원격 `https://github.com/dalihub/dali-web` refs와 로컬 `/home/jy/work/dali/dali-web` 존재·Git 상태를 재확인합니다. R1-a 확인 시 대상은 public, refs 없음, 로컬 경로 없음이었습니다. 기존 변경이 나타나면 덮어쓰지 않습니다.

1. 사용자 R1-b 실행 지시에서 로컬 commit 허용 여부 확인. subtree add 자체가 commit을 생성하므로 commit 금지 조건이 계속되면 실행하지 않습니다. 이번 R1-a에서는 import·이관·commit·push를 모두 제외했습니다.
2. R1-a 기준으로 README·제외 규칙·lock·개발자 remote 준비 스크립트·upstream 안내를 준비하고 repo 뼈대를 구성합니다. 개인 로컬 작업 트리를 복사하지 않습니다.
3. 아래 full SHA를 공개 원본에서 fetch하고 tree를 검증한 뒤 각각 `libs/<name>`에 `--squash` import합니다. tag object와 peeled commit을 혼동하지 않습니다. 각 생성 commit에는 당시 실제 모델명을 사용한 AI co-author trailer를 정확히 한 번 넣고 검증합니다.
4. 라이선스·third-party 고지·원본 tracked tree 보존, 중첩 `.git`·SDK·빌드 산출물 제외, lock과 subtree tree 일치를 확인합니다. tracked `libs/*/build/`와 autogen 소스는 제거 대상이 아닙니다.
5. 별도 로컬 일반 clone에서 세 소스·라이선스·lock을 확인합니다. README의 시작점과 remote 설정 스크립트의 반복 실행·URL 불일치 동작도 검증합니다.
6. 실제 repo 경로·commit·검증 결과·push 여부를 기록하고 R2-a로 넘깁니다. push는 실행 지시에 포함된 경우에만 수행합니다. 실험 코드 이관은 R2 범위입니다.

## 확정 기준

| 원본 URL | import commit | prefix |
|---|---|---|
| https://github.com/dalihub/dali-core.git | `80998009aee9a30ab4c7909c68281ed1983aa4ef` | `libs/dali-core` |
| https://github.com/dalihub/dali-adaptor.git | `d76afcfff7a09d788bb0cd3204aace153fce8cfe` | `libs/dali-adaptor` |
| https://github.com/dalihub/dali-ui.git | `13e6bf80ca8cef250a787bb2643899e4d9648cda` | `libs/dali-ui` |

Core·Adaptor 태그는 `dali_2.5.43`, UI 태그는 `v2.5.43.11572`. 최초와 후속 모두 squash입니다. 이력·tree·lock 세부 필드는 R1-a 결정 문서를 그대로 사용합니다. 실제 dali-web commit은 아직 없고 소스 업로드도 없습니다.

## 특이 사항과 보존할 것

- Core·Adaptor P0 SHA는 GitHub에서 찾을 수 없었지만 선택 공개 commit의 전체 tree가 각각 동일하며 diff 종료 0입니다. P0의 303/305개 이력 차이를 소스 차이로 해석하지 않습니다.
- UI는 P0 검증 대상이 아닙니다. 최신 devel은 2.5.44 계열이라 선택하지 않았습니다. 선정한 UI와 Core·Adaptor의 결합 빌드는 미검증이며 P6에서 확인합니다.
- `/home/jy/work/dali/dali-ui`는 사용자 작업 브랜치이며 `todo-items/README.md` 수정과 `todo-items/recycler-api-abi-refactoring.md` 미추적 파일을 보존했습니다. 이를 import하지 않습니다.
- 기록 기준은 여전히 `/home/jy/work/dali/webassembly-probe/docs`. R2-b 완료 전 기존 복제본·SDK·P0 로그를 삭제하거나 기록 기준을 전환하지 않습니다.
- 증거는 `logs/r1-a/`. 임시 조사 bare repo `/tmp/dali-r1a-ui-Tiv4hU`는 남아 있지만 필수 입력이 아닙니다. 실행 중인 조사·빌드·서버 프로세스는 남기지 않았습니다.

## 반복하지 않을 조사와 후속 재현

SDK 재설치, P0의 모의 그래픽 결과를 실제 픽셀 증거로 해석하는 작업은 하지 않습니다. Core 전체 Wasm 빌드·Node·Firefox 애니메이션, Adaptor 6개 번역 단위, GL/EGL 링크 탐색까지만 기존 검증입니다. 생산 렌더러·Windows·UI는 미검증입니다.

R2에서 필요할 때 기존 `build-core.sh`, `probe-adaptor.sh`, `probe-gl-symbols.py`, `check-browser.py`를 이관·실행합니다. 같은 이름의 기존 로그를 덮어쓰기 전에 별도 실행 디렉터리에 보존합니다. R1-b에서는 코드 변경 없는 라이브러리 빌드를 반복하지 않습니다.

이전 인수인계는 [004-R1-a](handoffs/004-R1-a.md)에 보관했습니다. 다음 완료 시 JOURNAL·STATE·PLAN을 맞추고 이 HANDOFF도 보관 후 교체합니다.
