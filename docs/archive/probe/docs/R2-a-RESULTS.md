# R2-a 이관·Core Wasm·Node 검증 결과

일자: 2026-10-23 (Asia/Seoul). R2-a 완료. R2-b 미착수이며 상위 R2는 진행 중입니다.

- 작업 repo: `/home/jy/work/dali/dali-web`, branch `main`.
- 시작 HEAD: `888e8e7e1409c1637f6cff8f1024e77c34b43728`, clean 확인.
- 생성 commit: `728cf818d4db74890a4c37d01737c8eea3d9a14b`.
- 원격: https://github.com/dalihub/dali-web. push 결과는 아래 원격 확인 기록 참고.
- upstream 갱신 없음. lock 불변: Core `e2ce797a41cff49851361210491c8ed38bea46e9`, Adaptor `f0dd9b39d5a51ff0233ea50bf872dd6200e93981`, UI `f6c046d2a15ab7c6bad8d76612e667d66ed1f6d0`.
- 증거 디렉터리: `../logs/r2-a/run-001/`. 기존 P0 로그와 분리.

## 이관 파일과 의존성

| 기존 파일 | 새 파일 | 변경 |
|---|---|---|
| CMakeLists.txt | dali-web/cmake/CMakeLists.txt | libs/dali-core 연결, binary 생성 헤더, Debug 정적 Core와 upstream linker-test의 모의 테스트 유틸리티 재사용 |
| core-smoke.cpp | tests/probes/core-smoke.cpp | 내용 동일, TestApplication·mock Graphics Controller에 의존 |
| core-smoke.html | tests/browser/core-smoke.html | 내용 동일, 빌드 경로로 복사 |
| build-core.sh | scripts/build-core.sh | 새 cmake 경로, 도구 오버라이드, 1~6 jobs, 실행별 명령·종료 코드·로그 보존 |
| env.sh | scripts/env.sh | 개인 경로 제거, 명시 SDK → 활성 PATH → repo tools/emsdk 탐색 |
| probe-adaptor.sh | scripts/probe-adaptor.sh | libs 경로와 Core 생성 include 사용; 이번 실행 없음 |
| probe-gl-symbols.py | scripts/probe-gl-symbols.py | build/run 환경변수 및 LLVM_NM·EMXX 탐색; 이번 실행 없음 |
| check-browser.py | tests/browser/check-browser.py | build/run 경로와 프로필 분리, GECKODRIVER 오버라이드; 이번 실행 없음 |

추가 파일: `toolchain.lock.json`, `docs/BUILDING.md`. 갱신: 새 repo README·docs/UPSTREAMS.md. Core `libs/dali-core/build/tizen/CMakeLists.txt`에 생성 헤더 경로 옵션 8줄 추가. 총 13개 파일, 410줄 추가·3줄 삭제. SDK 다운로드 복구용 `download-sdk.py`, SDK·host packages·P0 소스·산출물·전체 과거 문서는 이관하지 않았습니다. 현재 PLAN·STATE·HANDOFF·JOURNAL은 이 기존 공간에 유지합니다.

## 실행 명령과 결과

작업 공간 `/home/jy/work/dali`에서 실행:

```sh
DALI_WEB_EMSDK="$PWD/webassembly-probe/tools/emsdk" \
DALI_WEB_HOST_TOOLS="$PWD/webassembly-probe/tools/host/usr" \
DALI_WEB_RUN_DIR="$PWD/webassembly-probe/logs/r2-a/run-001" \
./dali-web/scripts/build-core.sh
```

driver stdout/stderr도 run-001/driver.log에 보존했습니다. 이 디렉터리 재사용은 스크립트가 거부하므로 재현 시 새 run 이름을 사용합니다. 전체 확장 명령·개별 종료 코드: commands.log (로컬 보존 자료: `webassembly-probe/logs/r2-a/run-001/commands.log`; [위치 안내](../../../MIGRATION.md)).

| 명령/검사 | 종료 코드 | 결과/증거 |
|---|---|---|
| emcc/node/cmake/ninja/python 버전 | 각 0 | Emscripten 6.0.13, Node 24.19.0, CMake 3.22.1, Ninja 1.10.1, Python 3.10.12 |
| emcmake cmake -S dali-web/cmake -B dali-web/build/core -G Ninja … | 0 | configure.log |
| cmake --build dali-web/build/core --parallel 6 | 0 | build.log, 375개 단계 완료 |
| node build/core/core/linker-test.js | 0 | linker-test.log |
| node build/core/core-smoke.js | 0 | core-smoke.log: midpoint=50.0 endpoint=100.0 |
| build-core.sh 전체 | 0 | driver.log |
| bash -n scripts/*.sh, Python AST 구문 검사, git diff --check | 0 | validation.log |
| DALI_WEB_JOBS=7 … | 2 (예상 거부) | validation.log의 검증 wrapper는 0 |
| 원본/P0 상태·hash, lock tree, 생성 헤더 위치·ignore 검사 | 0 | validation.log, preservation-before.json |

모든 빌드 스크립트 tee 종료 코드도 0입니다. 재사용 SDK의 emsdk revision은 `d7b4130688bfa348392b921ab04015aa15c4f154`, compiler revision은 `e13ee988ce3d87a738dd8f3661d02223308c412a`. SDK 설치를 새로 수행하지 않았습니다.

## 특이 사항·검증 한계

1. 현재 Core의 namespace 헤더 생성은 기본적으로 source tree를 씁니다. 선택적 `DALI_GENERATED_INCLUDE_DIR`을 추가하고 웹 빌드에서만 설정해 새 `build/core/generated/include`로 분리했습니다. 기존 네이티브 기본 동작은 유지합니다. public API·ABI 변경 없음. 네이티브 빌드와 install 실행은 미검증입니다.
2. 이번 성공은 현재 고정 Core SHA에 위 CMake 경로 변경을 더한 새 빌드의 결과입니다. P0 성공을 재사용하지 않았습니다. Core 구현 코드와 upstream lock은 변경하지 않았습니다.
3. 모의 그래픽·단일 스레드 애니메이션 결과이며 생산 GLES·실제 Canvas 픽셀·UI 결합·성능·Windows를 검증하지 않습니다. 새 Adaptor SHA의 번역 단위·GL 결과도 아직 미검증입니다.
4. sandbox의 최초 push는 DNS 오류로 종료 128. 네트워크 허용 환경에서 force 없이 재시도했습니다. 결과는 아래 확인 기록 참고.
5. 원본 Core·Adaptor·UI와 P0 두 복제본의 HEAD·status·tracked diff, 기존 로그와 UI 사용자 파일의 hash 불변을 검사했습니다. P0·SDK를 삭제/이관하지 않았고 기존 SDK를 재사용했습니다.

생성물은 새 repo의 ignored `build/core/`와 기존 공간의 `logs/r2-a/run-001/`입니다. 새 브라우저 프로필은 없으며 남긴 빌드·서버·브라우저 프로세스도 없습니다. SDK·산출물·대용량 로그는 commit에 포함하지 않았습니다. R2-b의 첫 행동은 [HANDOFF](HANDOFF.md)를 따릅니다.

## 원격·commit 최종 확인

- `git push origin main`: 최초 128(DNS), 네트워크 허용 재시도 0. push-network.log (로컬 보존 자료: `webassembly-probe/logs/r2-a/run-001/push-network.log`; [위치 안내](../../../MIGRATION.md)).
- `git ls-remote origin refs/heads/main`: 0, `728cf818d4db74890a4c37d01737c8eea3d9a14b` 일치. remote-head.log (로컬 보존 자료: `webassembly-probe/logs/r2-a/run-001/remote-head.log`; [위치 안내](../../../MIGRATION.md)).
- commit 본문의 `Co-Authored-By: Codex GPT-6` 정확히 1회 확인. 생성 commit은 1개입니다.
- 현재 상태 문서들은 기존 공간의 로컬 문서이며 Git commit에 포함되지 않습니다. 새 repo에는 빌드 사용 문서만 반영했습니다.
