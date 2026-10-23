# P1 작은 검증 증거

2026-10-23. P1 실행 디렉터리는 repo의 `logs/p1/run-001/`이며 ignored이다. 이 디렉터리에는 SDK/브라우저 산출물을 넣지 않았다. 생산 빌드·GL 호출·브라우저·pthread·호스팅 실험은 실행하지 않았다.

## SDK map access guard 검사

[check-map-guards.mjs](check-map-guards.mjs)는 외부 SDK의 `libwebgl.js`에서 glMapBufferRange의 access 검사 구문만 읽어 Node 함수로 실행한다. WebGL target 검사 이전까지만 실행하며 allocation·upload는 실행하지 않는다. [결과](map-guards.json), [버전](versions.json), [읽은 소스 hash](source-hashes.json).

SDK는 [BUILDING](../../BUILDING.md)의 lock 설치 또는 환경변수로 선택한다. 활성 Node가 lock 버전인지 먼저 확인한다. 재현 명령:

```sh
node --version
node docs/evidence/p1/check-map-guards.mjs "$DALI_WEB_EMSDK"
```

이번에는 기존 SDK Node 24.19.0, Emscripten 6.0.13을 사용했다. 원래 SDK 파일은 수정하지 않았다. 2(WRITE)는 반환 0, 6/10(WRITE+invalidate)은 guard 통과, 3/34(READ 또는 UNSYNCHRONIZED 포함)는 반환 0이다. 검사 exit 0은 예상 실패 경로 확인 성공이다. invalidate로 실제 GL 매핑이 성공하거나 byte가 보존된다는 증거가 아니다.

## 명령과 종료 코드

| 명령/작업 | 종료 코드 | 범위 |
|---|---|---|
| `git status --short --branch`, `git log -3 --oneline`, `git remote -v` | 각각 0 | 시작 clean main, HEAD ef6a1f545ff03df69c3297135048f545ee8dd352 |
| `git ls-remote origin refs/heads/main` (최초 sandbox) | 128 | DNS 실패, 대화 tool 출력에 보존 |
| 같은 remote 조회 (허용된 호스트 권한) | 0 | 시작 HEAD와 동일, upstream fetch 없음 |
| `emcc --version` | 0 | 6.0.13, lock 유지 |
| SDK Node로 `check-map-guards.mjs "$DALI_WEB_EMSDK"` | 0 | 5개 access 조합, GL 실행 없음 |
| `git --version`, `python3 --version`, SDK `git rev-parse HEAD` | 각각 0 | versions.json에 실제 값 기록 |
| `python3 logs/p1/run-001/preserve.py …` 전/후 | 각각 0 | 5개 원본/P0 repo HEAD·status·diff, 사용자 파일·과거 로그·R2-b 증거·lock·Core 산출물 136개 hash 비교 |
| 전/후 JSON 동일성·생산 diff 검사 Python | 0 | validation.json, productionDiffEmpty=true |
| 최초 근거 경로 검사 | 1 | 신규 제안 web 디렉터리를 기존 파일로 취급한 검사기 오류 |
| 수정한 문서 상대 링크·근거 경로·공백 검사 | 0 | 제안 경로 제외, validation.json의 개별 항목 |

명령은 repo 루트에서 실행했다. `commands.txt`의 변수 표기는 개인 경로를 제거한 표기다. 최초 remote 실패/성공은 대화 tool 출력에서 기록했으며 로컬 remote 재확인·commit/push receipt는 `logs/p1/run-001/`에 남긴다. 잘못 추정한 파일 경로의 초기 rg 탐색 오류는 생산 검증 실패로 집계하지 않았고, 실제 파일을 다시 찾아 설계에 사용했다.

보존 검사 스크립트와 전체 전후 snapshot은 로컬 로그에만 둔다. 기존 R2-b script는 읽기만 했고 P1 전용 사본으로 확장했다. [validation.json](validation.json)은 전후 불변과 검사 범위 요약이며, 원본 repo 사용자 diff를 공개하지 않는다. 전체 SDK hash 또는 새 다운로드 검증을 수행한 것은 아니다.

온라인 근거는 [P1 설계 6절](../../P1-EXECUTION-CONTRACT.md)에 URL·열람일·확인 한계를 기록했다. 공식 문서 조건과 SDK 구현을 조사했으며 실제 GitHub Pages 헤더·runtime·SW 재방문은 미검증이다. 새 실행 프로세스는 남지 않았다.
