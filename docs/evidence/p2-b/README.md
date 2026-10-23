# P2-b context 경계 중간 증거

2026-10-23. [결과](../../P2-b-RESULTS.md). 생산 Web host·생산 링크 성공 증거가 아니다.

- `commands.json`: 최종 `logs/p2-b/context-web-003`의 도구 버전, configure/Core build, 32개 compile/nm, 두 strict 링크의 argv/cwd/개별 exitCode.
- `native-commands.json`: 세 native backend configure 실패 및 native 추출 3개 cpp의 Emscripten 교차 compile/nm. native host 실행과 구분한다.
- `source-commands.json`: P2-a 원본 source 목록과 현재 목록의 GLES/VULKAN/DYNAMIC 비교, 잘못된 환경 조합의 예상 실패.
- `sources.txt`, `source-hashes.json`: 현재 32개 compile 대상과 관련 입력 hash. 전체 Core source/hash/compile_commands는 로컬 build/log에 보존했다.
- `nm-context-boundaries.json`: Controller/common Context/Web context/rendering의 전체 undefined 목록. EGL context API/RTTI가 없음을 확인; sync 등 전체 native 의존 제거 판정은 아님.
- `result.json`, `link-webgl2.txt`, `link-full-es3.txt`: strict exit 1, 미해결 13/9개, map 미생성. FULL_ES3의 4개 감소는 runtime/데이터 보존 근거가 아님.
- `versions.txt`, `validation.json`: 도구 실제 버전, 원본/P0/과거 증거/lock/기존 build 보존 검사.

`${DALI_WEB_ROOT}`, `${DALI_WEB_EMSDK}`, `${DALI_WEB_HOST_TOOLS}`는 공개용 경로 정규화다. 개인 경로를 빌드 필수 조건으로 만들지 않는다. 전체 로그는 ignored `logs/p2-b/`, 산출물은 `build/p2-b/`에 있다. `context-web-001` compile 실패, 002 성공, 최종 입력 기록을 더한 003 모두 보존했다. SDK·대형 로그·산출물은 commit 제외.
