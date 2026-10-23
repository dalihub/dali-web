# Core WebAssembly probe 빌드

Ubuntu x86_64에서 검증한 모의 그래픽 기반 Core 실행 검사입니다. 생산 Adaptor·UI·DALi 픽셀 렌더링은 포함하지 않습니다. 현재 소스 기준은 `upstreams.lock.json`이며 일반 clone만으로 소스가 준비됩니다. upstream fetch나 P0 복제본은 필요하지 않습니다.

## 도구 준비

검증 버전은 `toolchain.lock.json`에 기록합니다: Emscripten 6.0.13, SDK Node 24.19.0, CMake 3.22.1, Ninja 1.10.1, Python 3.10.12. 이 파일은 재현 기준이며 스크립트가 다른 버전 설치를 자동 변경하거나 거부하지는 않습니다. 실제 사용 버전은 실행 로그에서 확인합니다. 다른 버전·플랫폼은 미검증입니다.

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
