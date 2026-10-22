# Tizen 브랜치 기준으로 변경

2026-10-22 사용자 요청에 따라 R1-a의 릴리스 태그 기준을 대체합니다. Tizen 공식 원격의 `refs/heads/tizen` tip을 조회한 뒤 아래 full SHA로 고정했습니다. 자동으로 움직이는 branch tip을 빌드 기준으로 사용하지 않습니다.

| 라이브러리 | commit | tree | P0 tree 동일 |
|---|---|---|---|
| dali-core | `e2ce797a41cff49851361210491c8ed38bea46e9` | `4c24762a92a8b25e35b4c99e1b4cac47c161fd5a` | False |
| dali-adaptor | `f0dd9b39d5a51ff0233ea50bf872dd6200e93981` | `612a124b06eab2fe0c44cfc17f3ea1aefeb2f4d8` | False |
| dali-ui | `f6c046d2a15ab7c6bad8d76612e667d66ed1f6d0` | `7d5d851aebe2eb0e03473b8ea2b325c1be98e7fb` | None |

기준 URL은 `ssh://review.tizen.org:29418/platform/core/uifw/<name>`이며 개인 계정명을 lock에 넣지 않습니다. Tizen SSH 접근 권한과 사용자별 SSH 사용자명 설정이 필요합니다. 이번 HTTPS 조회는 403이었고 기존 SSH 계정으로 조회·fetch했습니다. 일반 dali-web clone에는 전체 소스가 포함되므로 Tizen 연결이 필요하지 않습니다.

최초 GitHub 릴리스 import 이력을 유지하고 각 subtree에 `merge --squash`를 적용했습니다. R1-a 스냅샷은 과거 결정으로 보존합니다. 현재 기준은 이 문서와 lock입니다. 기존 clone의 upstream remote URL은 자동으로 덮어쓰지 않으며, 기존 값을 확인한 뒤 다음과 같이 의도적으로 변경합니다.

```sh
git remote set-url upstream-dali-core ssh://review.tizen.org:29418/platform/core/uifw/dali-core
git remote set-url upstream-dali-adaptor ssh://review.tizen.org:29418/platform/core/uifw/dali-adaptor
git remote set-url upstream-dali-ui ssh://review.tizen.org:29418/platform/core/uifw/dali-ui
./scripts/setup-upstream-remotes.sh
```

필요하면 fetch 명령에 `GIT_SSH_COMMAND="ssh -l YOUR_TIZEN_USER"`를 지정하거나 개인 SSH config에서 사용자명을 설정합니다. 계정·키를 저장소에 추가하지 않습니다.

새 고정 소스의 빌드·Node·브라우저 검증은 아직 수행하지 않았습니다. 과거 P0 성공 결과를 새 SHA의 성공 결과로 간주하지 않습니다. R2-a에서 Core 빌드·Node를, R2-b에서 브라우저·GL 탐색을 검증합니다. 기록 기준은 R2-b 완료 전까지 기존 `webassembly-probe/docs`입니다.
