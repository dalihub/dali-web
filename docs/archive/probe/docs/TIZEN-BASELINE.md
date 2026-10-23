# R1-b 후속: Tizen tizen 브랜치로 기준 변경

2026-10-22 사용자 요청으로 세 원본의 기준을 Tizen 공식 `refs/heads/tizen` 최신 tip으로 변경했습니다. R1-a의 GitHub 릴리스 태그 기준을 대체합니다. 각 tip은 조회 시점의 full SHA로 고정하며 자동 추종하지 않습니다.

새 repo commit: `888e8e7e1409c1637f6cff8f1024e77c34b43728` (`main`, origin/main 및 원격 조회 일치). 기존 R1-b 이력 위에 squash merge 3회와 lock·문서 commit을 추가했고 force 없이 push했습니다.

| 라이브러리 | 고정 commit | tree | P0 tree 동일 |
|---|---|---|---|
| dali-core | `e2ce797a41cff49851361210491c8ed38bea46e9` | `4c24762a92a8b25e35b4c99e1b4cac47c161fd5a` | False |
| dali-adaptor | `f0dd9b39d5a51ff0233ea50bf872dd6200e93981` | `612a124b06eab2fe0c44cfc17f3ea1aefeb2f4d8` | False |
| dali-ui | `f6c046d2a15ab7c6bad8d76612e667d66ed1f6d0` | `7d5d851aebe2eb0e03473b8ea2b325c1be98e7fb` | None |

원본 URL은 `ssh://review.tizen.org:29418/platform/core/uifw/<name>`입니다. 개인 계정명은 lock에서 제외했습니다. 원본 갱신에는 Tizen SSH 계정·접근 설정이 필요하지만 일반 dali-web clone에는 세 소스가 포함되므로 해당 연결이 필요하지 않습니다.

## 변경·검증

- 전체 subtree를 `merge --squash`로 갱신하고 lock의 URL·ref·commit·tree·P0 비교를 맞춤. 기존 GitHub upstream remote만 기대 URL을 확인한 뒤 명시적으로 Tizen URL로 전환.
- 새 repo `docs/TIZEN-BASELINE.md`, README·UPSTREAMS·라이선스 목록 갱신. R1-a 원문 스냅샷과 기존 commit은 보존.
- 별도 일반 clone `/tmp/dali-tizen-verify-clone`에서 tree·파일 경로/모드·lock·라이선스·ignore·subtree 메타데이터·모든 생성 commit의 co-author trailer 검사 PASS.
- remote 스크립트의 반복 실행과 URL 충돌 시 무변경 확인. clone fsck 및 diff check 종료 0.
- 원본 저장소·P0 복제본의 HEAD·status 및 사용자 파일·기존 P0 로그 보존 확인.
- 세 갱신 각각의 ref·fetch·diff·merge 증거, 검증 JSON과 push 결과는 `../logs/r1-b-tizen/`에 보존.

## 특이 사항과 검증 한계

- HTTPS 원격 조회는 403, SSH 조회·fetch는 성공. 일시적 DNS 오류는 재시도로 해결.
- Core·Adaptor는 기존 P0와 tree가 다릅니다. P0 성공 결과를 새 고정 SHA의 검증 결과로 사용하지 않습니다. 세 라이브러리의 빌드·런타임 호환성은 이번에 검증하지 않았습니다.
- subtree 내부 squash commit은 기존 임시 메시지 wrapper로 trailer를 추가했고, merge commit은 명시적인 메시지를 사용했습니다. 설치된 Git 파일은 수정하지 않았습니다.
- 실험 코드 이관·빌드·Node·브라우저 실행은 수행하지 않았습니다. 다음은 R2-a이며 기록 기준은 R2-b 완료 전까지 기존 webassembly-probe/docs입니다.

[현재 인수인계](HANDOFF.md), [다음 프롬프트](NEXT-CONTEXT.md), 새 repo 기준 문서 (로컬 보존 자료: `dali-web/docs/TIZEN-BASELINE.md`; [위치 안내](../../../MIGRATION.md)).
