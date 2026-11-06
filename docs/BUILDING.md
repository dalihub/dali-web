# Core WebAssembly probe 빌드

현재 승인된 작업 기준은 [원본 Emception + Emscripten 3.1.24](PLAYGROUND-COMPILATION.md)다. [버전 정렬 검사](EMCEPTION-3.1.24-ALIGNMENT.md)를 통과했다. 기존 toolchain.lock.json의 6.0.13과 아래 R2 설치 예시는 과거 재현용으로 보존한다. 기존 자동 탐색은 유지하며 P3에서는 전용 선택 스크립트로 3.1.24를 명시한다. 3.1.24 결과는 전체 browser/GPU 지원 선언이 아니다.

로컬 재개 환경은 `logs/emception/alignment-3124-001/env.sh`, 이식용 참고는 [정규화한 환경 기록](evidence/emception-3.1.24-alignment/env.sh)이다. 로컬 수동 추출 SDK `tools/emsdk-3.1.24`에는 emsdk_env.sh가 없으므로 DALI_WEB_EMSDK로 지정하지 않는다. EM_CONFIG/EM_CACHE와 EMCC/EMXX/EMCMAKE/LLVM_NM/NODE를 명시하고 DALI_WEB_EMSDK를 비운 후 scripts/env.sh를 읽는다. 정규화 기록의 경로는 자신의 환경으로 치환한다. 실제 emcc 버전·config·cache를 먼저 확인하고 EMCC_CORES=6, 최대 -j6을 유지한다. 전용 lock은 `toolchain.emception-3.1.24.lock.json`이며 기존 lock을 덮어쓰지 않는다.

Ubuntu x86_64에서 검증한 모의 그래픽 기반 Core 실행 검사입니다. 생산 Adaptor·UI·DALi 픽셀 렌더링은 포함하지 않습니다. 현재 소스 기준은 `upstreams.lock.json`이며 일반 clone만으로 소스가 준비됩니다. upstream fetch나 P0 복제본은 필요하지 않습니다.


## 승인된 3.1.24 선택

준비된 SDK 루트에 upstream/emscripten, upstream/bin, .emscripten과 cache가 있어야 한다. 수동 SDK는 emsdk_env.sh가 필요 없다. 기존 config/cache를 수정하거나 SDK를 자동 설치하지 않는다. 다른 위치의 config/cache는 EM_CONFIG/EM_CACHE로 명시한다. config의 LLVM_ROOT/BINARYEN_ROOT/NODE_JS도 설치에 맞아야 한다.

```sh
export DALI_WEB_EMSDK_3124=/path/to/emsdk-3.1.24
export NODE=/path/to/node
# 추출형 host 도구를 쓰는 환경에서만 지정
export DALI_WEB_HOST_TOOLS=/path/to/host/usr
source scripts/env-emception-3.1.24.sh || exit 1
export DALI_WEB_BUILD_DIR="$PWD/build/p3/new-production"
export DALI_WEB_RUN_DIR="$PWD/logs/p3/new-production"
python3 scripts/check-web-production.py
```

선택 스크립트는 compiler revision을 검사하고 EMCC/EMXX/EMCMAKE/LLVM_NM을 같은 SDK로 고정한다. EMCC_CORES=6과 기본 DALI_WEB_JOBS=6을 사용한다. NODE·host 도구는 명시값/PATH를 사용하며 개인 경로가 필수는 아니다. DALI_WEB_TOOLCHAIN_LOCK은 생산 source hash 기록에 포함된다. 로컬 환경별 프로필은 ignored profiles/ 또는 logs/에만 보관한다.

P3 픽셀 계측 빌드는 위 생산 실행 전에 `export DALI_WEB_TEST_READBACK=ON`을 추가한다. 기본 OFF이며 readback 비용은 테스트 빌드에만 있다. 새 browser 로그 경로를 지정하고 다음을 실행한다. 두 방식 모두 loopback 임시 파일 서버만 사용하며 실제 Pages 배포는 하지 않는다.

```sh
export DALI_WEB_RUN_DIR="$PWD/logs/p3/new-browser-headers"
python3 tests/browser/check-production.py headers --pixels
export DALI_WEB_RUN_DIR="$PWD/logs/p3/new-browser-sw"
python3 tests/browser/check-production.py sw --pixels
```

Firefox/geckodriver와 loopback 소켓 실행 권한이 필요하다. `/probe/` 하위 경로에서 테스트하며 각 실행은 별도 프로필, 입력 hash, 상태·픽셀 JSON과 실제 판정을 남긴다. SW는 검사 전용 헤더 부여 구현이고 운영 캐시 정책이 아니다. CPU18/5 검사는 DALI_WEB_CORE_BUILD_DIR에 같은 SDK의 생산 빌드 경로를 지정하고 각각 새 build/run 경로에서 check-web-buffer-controller.py/check-web-gl.py를 실행한다. Node 전용 테스트 링크는 EXIT_RUNTIME=1로 pthread pool 종료를 명시한다.

## P3 GPU 메모리 후속 재현

위3.1.24 환경 선택 후 기존 경로를 덮어쓰지 않는 새 경로를 사용한다. `DALI_WEB_TEST_MEMORY`는 기본OFF이며 ON이면 두 프레임 readback도 함께 활성화된다. 실제 Controller/Core 업로드·GPU buffer/uniform readback·참조 수명을 검사한다.

```sh
export DALI_WEB_BUILD_DIR="$PWD/build/p3/new-memory"
export DALI_WEB_RUN_DIR="$PWD/logs/p3/new-memory"
export DALI_WEB_TEST_MEMORY=ON
python3 scripts/check-web-production.py
DALI_WEB_RUN_DIR="$PWD/logs/p3/new-memory-headers" python3 tests/browser/check-production.py headers --memory
DALI_WEB_RUN_DIR="$PWD/logs/p3/new-memory-sw" python3 tests/browser/check-production.py sw --memory
DALI_WEB_RUN_DIR="$PWD/logs/p3/new-memory-shader" python3 tests/browser/check-production.py headers --failure shader
DALI_WEB_RUN_DIR="$PWD/logs/p3/new-memory-reflection" python3 tests/browser/check-production.py headers --failure reflection
```

failure 모드는 같은 테스트 빌드에서 실제 malformed shader 또는 reflection 도중 invalid GL enum을 발생시킨다. GL 반환값을 주입하지 않는다. 정상 오류 진단은 남기며 ready→failure→stopped 및 trap 없음으로 판정한다. production 빌드는 `DALI_WEB_TEST_MEMORY=OFF`, `DALI_WEB_TEST_READBACK=OFF`로 별도 새 경로에서 실행한다. [실행 결과와 native 한계](P3-GPU-MEMORY-RESULTS.md).

## P3/P4 독립 C 이벤트 pthread 검사

사용자 승인 후 [app 이벤트 pthread + window 감독 구성](P3-EVENT-PTHREAD-RESULTS.md)의 조기 행렬을 완료했다. 기본 E는 유지하며 C는 기본OFF 테스트 target이다. 먼저 위3.1.24 환경을 선택하고 새 경로를 사용한다.

```sh
export DALI_WEB_C_BUILD="$PWD/build/p3/new-event-pthread"
"$EMCMAKE" "$CMAKE" -S cmake/web -B "$DALI_WEB_C_BUILD" -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM="$NINJA" \
  -DDALI_WEB_TEST_EVENT_PTHREAD=ON -DDALI_WEB_TEST_READBACK=ON
"$CMAKE" --build "$DALI_WEB_C_BUILD" --target dali-web dali-web-event-probe -j6
DALI_WEB_BUILD_DIR="$DALI_WEB_C_BUILD/event-probe" \
  DALI_WEB_RUN_DIR="$PWD/logs/p3/new-c-headers" \
  python3 tests/browser/check-production.py headers --event-pthread
DALI_WEB_BUILD_DIR="$DALI_WEB_C_BUILD/event-probe" \
  DALI_WEB_RUN_DIR="$PWD/logs/p3/new-c-sw" \
  python3 tests/browser/check-production.py sw --event-pthread
DALI_WEB_BUILD_DIR="$DALI_WEB_C_BUILD" \
  DALI_WEB_RUN_DIR="$PWD/logs/p3/new-e-regression" \
  python3 tests/browser/check-production.py headers --pixels
```

C 행렬은 정상→초기화 무한 루프→timer callback 무한 루프→새 정상 실행이다. 실제 픽셀·TLS/입력 signal·main heartbeat·소속2개 pthread counter 정지와 Canvas 제거·새 Module 렌더를 판정한다. 강제 종료는 소멸자 실행/브라우저 GPU 메모리 회수 보장이 아니다. [증거와 제한](evidence/p3-event-pthread/README.md)을 따른다.

`scripts/check-event-pthread.py`는 기존 검증된3.1.24 production 명령/입력 hash를 `DALI_WEB_PRODUCTION_COMMANDS`와 `DALI_WEB_PRODUCTION_HASHES`로 지정해 renderer/Core 객체를 재사용하는 로컬 진단 보조 도구다. 새 clone은 위 전체 CMake 경로를 사용한다. 기존 일반 dedicated Worker 시작 진단 `--event-worker`의 [Canvas 실패 기록](P3-EVENT-WORKER-RESULTS.md)은 보존하며 새 C 구성과 구별한다.

## 과거 R2 재현 도구 준비 (6.0.13)

검증 버전은 `toolchain.lock.json`에 기록합니다: Emscripten 6.0.13, SDK Node 24.19.0, CMake 3.22.1, Ninja 1.10.1, Python 3.10.12. 이 파일은 재현 기준이며 스크립트가 다른 버전 설치를 자동 변경하거나 거부하지는 않습니다. 실제 사용 버전은 실행 로그에서 확인합니다. 3.1.24의 추가 검증 범위는 위 정렬 결과를 따릅니다. 다른 플랫폼은 미검증입니다.

기존 SDK가 있으면 `DALI_WEB_EMSDK`에 emsdk 루트를 지정합니다. 새 설치는 emsdk의 고정 revision과 버전을 사용합니다. 아래 설치는 네트워크가 필요하며 R2-a에서는 기존 설치를 재사용했습니다.

```sh
git clone https://github.com/emscripten-core/emsdk.git tools/emsdk
git -C tools/emsdk checkout d7b4130688bfa348392b921ab04015aa15c4f154
./tools/emsdk/emsdk install 6.0.13
./tools/emsdk/emsdk activate 6.0.13
```

CMake·Ninja·Python 3는 호스트에 별도로 준비합니다. 기존 P0의 수동 분할 다운로드 스크립트는 설치 장애 복구용이어서 이관하지 않았습니다. SDK archive SHA256은 기존 다운로드 증거에서 가져온 값이며 이번에 새 다운로드를 검증한 결과는 아닙니다.

## 탐색과 오버라이드

`scripts/env.sh`의 탐색 순서:

1. `DALI_WEB_HOST_TOOLS`가 지정되면 그 아래 `bin`과 `lib/x86_64-linux-gnu`를 호스트 탐색 경로 앞에 추가합니다. 기존 추출형 Ubuntu 도구 재사용을 위한 선택 옵션입니다.
2. `DALI_WEB_EMSDK`가 지정되면 해당 `emsdk_env.sh`를 읽습니다. 없으면 활성화된 PATH의 `emcmake`를 사용하고, 그것도 없으면 repo의 `tools/emsdk/emsdk_env.sh`를 읽습니다.
3. `CMAKE`, `NINJA`, `NODE`, `PYTHON`, `EMCC`, `EMXX`, `EMCMAKE`, `LLVM_NM`의 명시 값이 우선합니다. 없으면 PATH에서 탐색하고, llvm-nm만 추가로 emcc의 형제 `../bin`을 탐색합니다. 직접 지정할 때는 실행 파일의 절대 경로를 사용합니다. 서로 호환되는 동일 SDK의 도구를 선택해야 합니다.

어떤 개인 절대 경로도 기본값이나 필수 조건이 아닙니다. SDK를 source하면 해당 SDK가 설정한 Node가 PATH 탐색에 사용됩니다. `NODE`를 지정하면 Node 실행 검사에 우선 적용됩니다.

## 실행

repo 루트에서:

```sh
./scripts/build-core.sh
```

외부 도구·출력 경로 예시(자신의 경로로 대체):

```sh
DALI_WEB_EMSDK=/path/to/emsdk \
DALI_WEB_HOST_TOOLS=/path/to/host/usr \
DALI_WEB_BUILD_DIR="$PWD/build/core" \
DALI_WEB_RUN_DIR="$PWD/logs/r2-a/my-run" \
DALI_WEB_JOBS=6 ./scripts/build-core.sh
```

기본 빌드 경로는 `build/core`, 로그는 매번 새 `logs/r2-a/<UTC timestamp>-<pid>`입니다. 지정 경로는 절대 경로를 권장합니다. `DALI_WEB_JOBS`는 1~6만 허용합니다. 같은 실행 로그 디렉터리에 `commands.log`가 있으면 덮어쓰지 않고 실패합니다. 소스·도구·빌드 옵션을 바꿀 때는 새 빌드 디렉터리를 사용합니다.

스크립트는 버전 확인 → `emcmake cmake -S cmake -B … -G Ninja` → Debug 정적 Core·검사 실행 파일 빌드 → Node linker-test → Node core-smoke 순서로 실행합니다. 각 명령과 종료 코드, tee 종료 코드가 `commands.log`에 남습니다. 실패 시 즉시 종료합니다.

성공 판정은 C++ 검사 종료 0과 `midpoint=50.0 endpoint=100.0`입니다. 500ms와 추가 501ms의 모의 프레임을 진행하며 허용 오차는 0.1입니다. 단일 스레드 테스트이고 실제 시간·브라우저 프레임 성능·픽셀을 검증하지 않습니다.

## 생성물과 Core 변경

`build/core/core/libdali2-core.a`, `core/linker-test.js`·`.wasm`, `core-smoke.js`·`.wasm`, `core-smoke.html`이 생성됩니다. namespace 헤더는 `build/core/generated/include/dali/public-api/common/dali-namespace.h`, InvokeMethod 코드는 `build/core/core/generated/invoke-method/`에 생성됩니다. 설치는 실행하지 않습니다.

Core CMake에 선택적 `DALI_GENERATED_INCLUDE_DIR`을 추가했습니다. 웹 CMake만 이를 설정하며 기존 네이티브 기본 경로는 유지합니다. 생성 include 경로를 소비자에게 전달하고 생성 헤더의 설치 위치를 유지합니다. public C++ API·ABI 변경은 없습니다. 이 옵션의 네이티브 빌드·설치 실행은 R2-a에서 검증하지 않았습니다.

SDK·빌드·로그·프로필은 루트 ignore 대상입니다. 외부 출력 경로를 사용하면 해당 경로의 보존·제외 여부는 사용자가 관리합니다.

## R2-b 브라우저·Adaptor·GL 검사

현재 lock 기준으로 Firefox 50/100·Adaptor 6개 컴파일·GL 탐색 비교를 완료했습니다. [결과](R2-b-RESULTS.md). Firefox 157.0.1과 geckodriver 0.37.1을 사용했습니다. Firefox·geckodriver는 별도 설치하며 PATH로 찾고, 드라이버는 `GECKODRIVER`로 재정의할 수 있습니다. 로컬 HTTP 소켓과 브라우저 실행 권한이 필요합니다. 제한된 sandbox에서는 실행 전에 실패할 수 있습니다.

repo 루트에서 새 실행 디렉터리와 Core 빌드 경로를 지정합니다. 아래 SDK/host 경로는 선택 옵션이며 자신의 설치 경로로 바꿉니다. 활성 SDK가 PATH에 있으면 두 export를 생략할 수 있습니다.

```sh
export DALI_WEB_EMSDK=/path/to/emsdk
export DALI_WEB_HOST_TOOLS=/path/to/host/usr
export DALI_WEB_BUILD_DIR="$PWD/build/core"
export DALI_WEB_RUN_DIR="$PWD/logs/r2-b/my-new-run"
# mkdir이 실패하면 기존 실행 디렉터리이므로 새 이름을 선택합니다.
mkdir -p "$(dirname "$DALI_WEB_RUN_DIR")"
mkdir "$DALI_WEB_RUN_DIR" || exit 1
source scripts/env.sh

# 현재 소스로 Core 빌드가 없으면 먼저 build-core.sh를 별도 run 경로로 실행합니다.
"$PYTHON" tests/browser/check-browser.py > "$DALI_WEB_RUN_DIR/browser.log" 2>&1
browser_code=$?
printf 'browser exitCode=%s\n' "$browser_code" >> "$DALI_WEB_RUN_DIR/commands.log"
test "$browser_code" -eq 0 || exit "$browser_code"
bash scripts/probe-adaptor.sh > "$DALI_WEB_RUN_DIR/adaptor.log" 2>&1
adaptor_code=$?
printf 'adaptor exitCode=%s\n' "$adaptor_code" >> "$DALI_WEB_RUN_DIR/commands.log"
test "$adaptor_code" -eq 0 || exit "$adaptor_code"
"$PYTHON" scripts/probe-gl-symbols.py > "$DALI_WEB_RUN_DIR/gl.log" 2>&1
gl_probe_code=$?
printf 'GL probe exitCode=%s\n' "$gl_probe_code" >> "$DALI_WEB_RUN_DIR/commands.log"
test "$gl_probe_code" -eq 0 || exit "$gl_probe_code"
cat "$DALI_WEB_RUN_DIR/gl-symbols.json"
```

Adaptor 검사는 Core 생성 헤더를 사용하고 6개 파일을 순차 컴파일합니다. 각 명령·종료 코드는 `adaptor/commands.log`에, 진단은 각 unit 로그에 남습니다. GL 탐색은 그중 두 object에서 직접 GL/EGL import를 추출합니다. 개별 링크 명령/종료 코드는 `gl-commands.log`, 진단과 집계는 `gl-symbols-*.log`, `gl-symbols.json`에 남습니다. 같은 디렉터리를 재사용하면 결과 파일이 덮어써질 수 있으므로 반드시 새 디렉터리를 사용합니다.

Python 종료 0은 탐색 결과 수집 성공입니다. 현재 두 링크 모두 종료 1이며, 기본 7개/FULL_ES3 3개 미해결입니다. placeholder 선언은 링크 전용이며 생성 코드를 실행하거나 API wrapper로 사용하지 않습니다. 미해결 심볼 해소·실제 호출 의미·전체 Adaptor 링크·실제 픽셀은 후속 단계입니다.

R2-b 실제 실행에서는 `logs/r2-b/run-001/`을 사용했습니다. 실제 명령·도구 버전·실패한 sandbox 시도까지 보존했고 repo에는 [작은 증거](evidence/r2-b/commands.txt)만 포함합니다. 전체 로그와 이전 공간 위치는 [MIGRATION](MIGRATION.md)을 참고합니다.


## 실제 생산 WebGL2 host (P2)

repo 루트에서 활성 SDK/PATH 또는 선택적인 DALI_WEB_EMSDK·DALI_WEB_HOST_TOOLS를 설정한 뒤 새 경로를 사용한다. 개인 SDK 절대 경로는 필수가 아니다.

```sh
source scripts/env.sh
export DALI_WEB_BUILD_DIR="$PWD/build/p2-b/my-production-run"
export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/my-production-run"
export DALI_WEB_JOBS=6
"$PYTHON" scripts/check-web-production.py
```

두 경로가 이미 존재하면 거부한다. cmake/web는 실제 graphics file.list로 Core·Controller·자원·명령·host를 빌드한다. FULL_ES3=0, ERROR_ON_UNDEFINED_SYMBOLS=1, ABORTING_MALLOC=0, pthread, OffscreenCanvas와 비차단 main 설정을 최종 링크에 유지한다. 산출물 dali-web.js/wasm과 index.html/bootstrap.js는 build 경로, 각 명령·종료 코드·버전·source hash·nm·map은 log 경로에 남는다. 전체 Tizen Adaptor WEB profile은 지원하지 않는다.

[production-003 증거](evidence/p2-b-production/README.md)의 명령 파일에는 Controller18·wrapper5 CPU 검사와 native/source 회귀 재현 입력도 있다. CPU 검사는 GL 응답을 주입하며 실제 GPU 성공이 아니다. native configure는 현재 host compiler 부재로 실패했다. 위 명령은 서버를 만들거나 브라우저/Pages를 실행하지 않는다. 최초 실제 pthread/browser의 헤더/SW 비교와 GPU/readback, 비동기 failure/stop/재생성은 P3/P4 실행 게이트다. 전역 Core/STL OOM 처리 완료를 의미하지 않는다.
