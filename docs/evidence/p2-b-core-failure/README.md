# P2-b Core 실패 처리 중간 증거

[결과·한계·후속 구현](../../P2-b-CORE-FAILURE-RESULTS.md). 2026-10-23. 사용자 승인 후 Core internal 실패 처리 검증이며 WebBufferMemory·실제 host·생산 링크·GPU 성공 증거가 아니다.

- `unit-commands.json`, `unit-source-hashes.json`, `unit-result.txt`: 최종 core-failure-005의 375단계 build, 실제 Core + mock Controller/Memory 실패 주입, 정상 애니메이션 50/100. 모두 exit 0.
- `boundary-commands.json`, `boundary-source-hashes.json`, `sources.txt`, `versions.txt`: core-failure-web-001의 pthread Core 333단계, 34개 생산 GLES compile/nm 각각 0. 전체 compile_commands와 nm-core 로그는 로컬 산출물에 보존.
- `link-webgl2.txt`, `link-full-es3.txt`, `result.json`: strict 기본/FULL_ES3 각각 exit 1, 미해결 10/6개, map 없음. 기존 sync 결과와 같은 링크 경계다. 실제 host 없음.
- `source-commands.json`: GLES/VULKAN/DYNAMIC source 회귀, Vulkan 목록 동일, 잘못된 환경 조합 거부.
- `native-commands.json`: native configure 3개 compiler 부재로 각각 exit 1. native 추출 코드 5개 Emscripten compile/nm 각각 0이며 변경 Core의 native build/install/runtime은 검증하지 못했다.
- `session.json`, `validation.json`: 실제 HEAD/원격 설정·증거 조회 위치, 검증 입력 일치·기존 증거·lock 보존 및 문서 검사. commit/push 없음.

경로는 `${DALI_WEB_ROOT}`, `${DALI_WEB_EMSDK}`, `${DALI_WEB_HOST_TOOLS}`로 정규화했다. 도구 경로는 `scripts/env.sh` 설정으로 바꾸며 개인 SDK 경로를 요구하지 않는다. 전체 로그는 `logs/p2-b/core-failure-{001,002,003,004,005,web-001,native-001,sources-001}`, 대응 산출물은 `build/p2-b/`에 보존한다. 실패한 001/002/004를 성공 로그로 덮어쓰지 않았다.

재현: SDK·host 도구 경로를 설정하고 `source scripts/env.sh` 뒤 새 `DALI_WEB_BUILD_DIR`/`DALI_WEB_RUN_DIR`로 `python3 scripts/check-core-buffer-failure.py`를 실행한다. 생산 객체 진단은 `scripts/check-gles-boundary.py`, source/native 검사는 기존 스크립트를 사용한다. 정확한 명령은 위 JSON을 따른다. 최대 -j6, SDK·산출물·대용량 로그 commit 제외.
