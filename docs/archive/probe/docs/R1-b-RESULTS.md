# R1-b 저장소 구성 결과

일자: 2026-10-22 (Asia/Seoul). 상태: R1-b 및 상위 R1 완료.

> 이후 사용자 요청으로 [Tizen 기준](TIZEN-BASELINE.md)으로 갱신했습니다. 아래 commit과 검증은 최초 R1-b 완료 시점의 기록입니다.

## 결과

- repo: `/home/jy/work/dali/dali-web`, remote: https://github.com/dalihub/dali-web.
- branch: `main`, 최종 commit: `888ff1423ca205bbaf65c493ee8d5771257f9a6a`. `origin/main` 및 원격 조회 SHA 일치. force 없이 push 완료.
- README·루트 생성물 제외 규칙·`upstreams.lock.json`·`scripts/setup-upstream-remotes.sh`·upstream 관리 및 라이선스 목록 구성.
- R1-a 공개 URL·full SHA·tree로 Core·Adaptor·UI 전체 tracked tree를 각각 `--squash` import. import 직후 및 별도 clone에서 lock tree 일치.
- R1-a 결정과 당시 HANDOFF 원문을 `dali-web/docs/r1-a/`에 역사적 스냅샷으로 포함. 원문 로컬 증거 링크의 위치는 해당 README에 명시. 현재 기록 기준은 이 `webassembly-probe/docs`.

## commit

| 구분 | commit |
|---|---|
| 뼈대 | `2dbad9b92e133e8ca3954913d0172f640a573b3f` |
| Core squash | `d7583d1e0733a377b922a95bbd26cfbb5b34b96d` |
| Core import | `31c709e820691b650863dc9ebffc79f4214b4b9a` |
| Adaptor squash | `137698c89000aa370645946124b119c0d881661a` |
| Adaptor import | `fc84617e92b0d1536810600856a2438bfecc1c95` |
| UI squash | `c9bef59e91d95b14d88391650a2c7f9f49a77f32` |
| UI import | `6ca017e3098c763857cf530a972458480fd55f2d` |
| 라이선스 목록 | `888ff1423ca205bbaf65c493ee8d5771257f9a6a` |

생성한 8개 commit 모두 `Co-Authored-By: Codex GPT-6` trailer를 정확히 한 번 포함. 원본 이력을 부모로 포함하지 않으며 일반 clone에는 위 8개 commit만 도달 가능합니다. 로컬 fetch로 확보한 상세 upstream 이력은 push 대상이 아닙니다.

## 검증과 증거

1. 대상 경로 없음 및 원격 refs 없음 재확인 후 생성. 사전 상태 (로컬 보존 자료: `webassembly-probe/logs/r1-b/preflight.txt`; [위치 안내](../../../MIGRATION.md)).
2. 고정 공개 URL에서 full SHA fetch 종료 0, 각 tree 고정값 일치. Core (로컬 보존 자료: `webassembly-probe/logs/r1-b/fetch-dali-core.txt`; [위치 안내](../../../MIGRATION.md)), Adaptor (로컬 보존 자료: `webassembly-probe/logs/r1-b/fetch-dali-adaptor.txt`; [위치 안내](../../../MIGRATION.md)), UI (로컬 보존 자료: `webassembly-probe/logs/r1-b/fetch-dali-ui.txt`; [위치 안내](../../../MIGRATION.md)).
3. 세 `subtree add --prefix=libs/<name> <full-sha> --squash` 종료 0. import (로컬 보존 자료: `webassembly-probe/logs/r1-b/import.txt`; [위치 안내](../../../MIGRATION.md)).
4. `git clone --no-local dali-web /tmp/dali-r1b-verify-clone` 종료 0. submodule·shared object alternates 없이 별도 일반 clone. Core 1,151 / Adaptor 1,820 / UI 3,172 파일, 전체 tree·경로·모드·lock 일치. 라이선스 파일 13개 내용 검사, 전체 tree 비교로 소스 내부 고지도 보존 확인. 검증 JSON (로컬 보존 자료: `webassembly-probe/logs/r1-b/clone-verification.json`; [위치 안내](../../../MIGRATION.md)), 출력 (로컬 보존 자료: `webassembly-probe/logs/r1-b/clone-verification.txt`; [위치 안내](../../../MIGRATION.md)), 검증 코드 (로컬 보존 자료: `webassembly-probe/logs/r1-b/verify-clone.py`; [위치 안내](../../../MIGRATION.md)).
5. remote 준비 스크립트: repo 밖에서 실행 성공, 반복 실행 시 config 동일. 앞 remote가 없고 뒤 remote URL이 충돌하는 경우 종료 1 및 전체 config 불변. 충돌 테스트 후 검증 clone의 remote를 정상값으로 복원.
6. 루트 생성물 ignore, tracked `libs/*/build/` 유지, gitlink·중첩 `.git` 없음 확인. `bash -n`, clone `git fsck --full`, `git diff --check` 종료 0.
7. 원본 3개 및 P0 복제본 2개의 HEAD·status, UI 사용자 파일·기존 P0 로그 hash 불변. 시작 스냅샷 (로컬 보존 자료: `webassembly-probe/logs/r1-b/preservation-before.json`; [위치 안내](../../../MIGRATION.md)), 보존 확인 (로컬 보존 자료: `webassembly-probe/logs/r1-b/preservation-after.txt`; [위치 안내](../../../MIGRATION.md)).
8. push 직전 refs 없음 재확인, `git push --set-upstream origin main:main` 종료 0, 원격 main SHA 검증. push 증거 (로컬 보존 자료: `webassembly-probe/logs/r1-b/push.txt`; [위치 안내](../../../MIGRATION.md)).

## 특이 사항

- sandbox DNS 실패는 네트워크 허용 실행으로 해결했습니다. 원본 URL·SHA·설계 변경은 없습니다.
- 설치된 Git 2.34.1 subtree는 `git commit-tree`로 squash와 merge commit을 생성하여 일반 commit hook을 거치지 않습니다. 설치 파일은 그대로 두고 `/tmp/dali-r1b-subtree` 복사본의 commit-tree 호출만 메시지 검증 wrapper로 연결했습니다. 생성 전에 trailer를 삽입·검증하고, 생성 후 8개 메시지와 subtree 메타데이터를 재검증했습니다. 이력 rewrite·amend·force push는 하지 않았습니다.
- wrapper `/tmp/dali-r1b-commit-tree.py`, import 실행 파일 `/tmp/dali-r1b-import.py` 및 검증 clone은 남아 있으나 R2-a의 필수 입력은 아닙니다.
- UI는 P0 검증 대상이 아니며 Core·Adaptor와 결합 빌드는 P6에서 확인합니다. R1-b의 소스 검증은 빌드·런타임 호환성을 보장하지 않습니다.

## 범위와 다음 단계

실험 코드·CMake·toolchain 이관, SDK 설치, 빌드·Node·브라우저 실행, Pages 활성화는 하지 않았습니다. P0 복제본·로그·SDK 및 원본 사용자 변경을 보존했고 실행 중인 작업 프로세스를 남기지 않았습니다.

다음은 [R2-a 인수인계](HANDOFF.md). 기록 기준은 R2-b 완료 전까지 기존 공간이며 새 repo에는 현재 상태 문서를 중복 관리하지 않습니다.
