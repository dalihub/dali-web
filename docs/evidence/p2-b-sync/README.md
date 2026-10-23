# P2-b sync 경계 중간 증거

2026-10-23. [결과](../../P2-b-SYNC-RESULTS.md). 실제 host·생산 링크·GPU 실행 성공 증거가 아니다. 이전 `p2-b/` 증거를 덮어쓰지 않고 별도 디렉터리에 남긴다.

- `commands.json`, `versions.txt`: 최종 sync-web-003의 도구 버전, configure/Core 333단계, 34개 compile/nm, 두 strict 링크의 명령·개별 종료 코드.
- `sources.txt`, `source-hashes.json`: 선택 closure, 관련 헤더·CMake·검증 코드 hash. 전체 Core 입력/compile_commands는 로컬 build/log에 있다.
- `nm-sync-boundaries.json`: Controller/common Context/SyncPool/texture dependency와 Web sync/context의 undefined 전체 목록. concrete EGL/NativeFence 의존 없음. 아직 native wrapper에는 eglGetProcAddress가 있다.
- `result.json`, `link-webgl2.txt`, `link-full-es3.txt`: exit 1·10/6개 미해결, map 미생성. 기본/FULL_ES3 모두 생산 링크 실패다.
- `unit-commands.json`, `unit-result.txt`: 실제 WebFence 코드에 GL 응답을 주입한 Node 수명 단위 검사. 테스트에만 linker `--wrap`을 사용하며 생산 진단은 사용하지 않는다. 실제 GPU/브라우저 성공으로 해석하지 않는다.
- `source-commands.json`: GLES/VULKAN/DYNAMIC source 비교·잘못된 조합 거부.
- `native-commands.json`: native configure 3개 실패 및 추출 코드 5개의 Emscripten 교차 compile/nm. native runtime은 미검증.
- `session.json`, `validation.json`: 시작 Git/원격 조회, 현재 세션 모델 확인, 과거 증거/산출물/원본 repo/lock 보존, 최종 입력 hash 일치.

`${DALI_WEB_ROOT}`, `${DALI_WEB_EMSDK}`, `${DALI_WEB_HOST_TOOLS}`는 경로 정규화다. 개인 절대 경로는 필수 조건이 아니다. 전체 로그는 `logs/p2-b/{sync-web-001,sync-web-002,sync-web-003,fence-unit-001,fence-unit-002,sync-native-001,sync-native-002,sync-sources-001,sync-session-001}`, 산출물은 별도 `build/p2-b/`에 보존한다. SDK·산출물·프로필·대용량 로그는 commit에서 제외한다.
