# 이벤트 Worker 시작 게이트 증거

2026-11-06. [판정·원인·중단 조건·후속 제안](../../P3-EVENT-WORKER-RESULTS.md).

- `e-baseline-*`: 동일 생산 runtime의 기존 E 재실행, ready→stopped, exit 0.
- `c-fixture-error-*`: 모듈 스크립트 URL 누락으로 nested Worker 로딩 실패, exit 1. 중간 fixture 오류이며 Canvas 실패 근거와 구별한다.
- `c-startup-*`: 명시 mainScriptUrlOrBlob 후 실제 pthread/Canvas 전달 실패, failure→stopped, exit 1. Core 생성·GPU 실행 이전이다.
- `*-result.json`: 브라우저 실제 관측값. cleanupRequested는 회수 완료가 아니다.
- `*-verdict.json`: 성공 gate는 false로 유지. 실패 재현을 통과로 바꾸지 않는다.
- `*-inputs.json`: 각 실행 fixture와 JS/Wasm/pthread Worker SHA256. 개인 경로만 `${DALI_WEB_ROOT}`로 치환했다.
- `validation.json`: 실제 시작 HEAD/원격/clean, 보존 748개·생산 499개·runtime 3개 drift 0, 개별 exit와 미검증 항목.

전체 로그·프로필은 `logs/p3/event-worker-{e-baseline-001,c-startup-001,c-startup-002}`. 기존 계측 없는 `build/p3/production-memory-final-001`을 재사용했으며 재빌드하지 않았다. SDK·대용량 산출물·프로필은 commit 대상이 아니다. Firefox 157.0.1의 직접 COOP/COEP 로컬 검사이며 SW/Pages 성공 근거가 아니다.

재현: [BUILDING](../../BUILDING.md)의 3.1.24 생산 산출물을 준비하고 새 로그 경로로 실행한다. SDK 설치/전환은 수행하지 않는다.

```sh
export DALI_WEB_BUILD_DIR="$PWD/build/p3/production-memory-final-001"
DALI_WEB_RUN_DIR="$PWD/logs/p3/new-e-baseline" python3 tests/browser/check-production.py headers
DALI_WEB_RUN_DIR="$PWD/logs/p3/new-c-startup" python3 tests/browser/check-production.py headers --event-worker
```

두 번째 명령은 현재 exit 1이다. 일반 clone에서는 기존 로컬 산출물이 없으므로 BUILDING의 계측 없는 생산 빌드를 새 경로에 준비해야 한다. 이 시작 검사는 무한 루프, TLS/이벤트 소유권, 전체 pthread 종료, 새 인스턴스 재생성의 인수 테스트를 대신하지 않는다.
