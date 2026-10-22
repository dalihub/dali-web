# Upstream 관리

세 라이브러리는 `upstreams.lock.json`의 공개 URL·full SHA에서 전체 tracked tree를 `libs/<name>`으로 가져옵니다. 최초와 후속 모두 `--squash`입니다. 원본 상세 이력은 upstream에서 조회합니다. 기준 ref는 참고용이며 자동으로 branch tip을 import하지 않습니다.

## 개발자 remote

`scripts/setup-upstream-remotes.sh`는 lock을 읽고 없는 remote만 추가합니다. 같은 URL이면 유지하고 다른 URL이나 별도 push URL이 있으면 변경 전에 실패합니다. 자동 네트워크 작업은 하지 않습니다. 원본 관찰 branch는 Core·Adaptor master, UI devel입니다.

## 후속 갱신

한 라이브러리씩 URL·후보 full SHA·tree를 검토한 뒤 fetch합니다. `git subtree merge --prefix=libs/<name> <full-sha> --squash`로 갱신하고 충돌 해결·회귀 검증 후 lock을 함께 갱신합니다. 포팅 변경은 import와 별도 commit으로 남깁니다. `git-subtree-dir`·`git-subtree-split` 메타데이터는 보존합니다.

자동 생성 squash commit도 commit 정책의 적용 대상입니다. 일반 commit hook이 `git commit-tree`에는 적용되지 않으므로 실행 전 메시지 삽입 절차를 준비하고 모든 생성 commit을 검사해야 합니다.

## 라이선스와 제외 규칙

각 upstream의 LICENSE·COPYING·NOTICE·저작권 및 third-party 고지는 원래 경로와 내용으로 유지합니다. 전체 tree 검증은 고지뿐 아니라 소스 내 저작권 주석도 포함합니다. 루트에서 다른 라이선스로 일괄 재지정하지 않습니다. [라이선스 파일 목록](LICENSE-INVENTORY.md)을 함께 확인합니다.

루트 `.gitignore`는 tools·build·logs·profiles·browser-profiles·dist만 제외합니다. `libs/*/build/`의 tracked 빌드 소스와 tracked autogen 파일은 원본 tree의 일부입니다. 중첩 `.git`, SDK, P0 생성 헤더와 산출물을 파일 복사로 섞지 않습니다.

Core·Adaptor는 P0와 tree가 같지만 UI는 P0 검증 대상이 아니며 결합 빌드는 P6에서 검증합니다. 상세 선택 근거는 [R1-a 결정 스냅샷](r1-a/R1-a-UPSTREAMS.md)에 있습니다.
