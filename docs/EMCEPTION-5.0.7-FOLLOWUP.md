# Emception 5.0.7 추가 검증

2026-10-26. [예비 검사](EMCEPTION-5.0.7-PRELIMINARY.md) 이후 사용자 요청에 따라 공식 실행 구성, 상대 경로, 제한시간, 라이브러리 사전 로딩, JavaScript 생성 경로를 추가 확인했다. **일반 C++의 브라우저 컴파일·실행은 확인했지만, 이 배포물로 DALi용 Emscripten JavaScript/pthread/WebGL 출력까지 만들 수 있다는 게이트는 통과하지 못했다.** 따라서 SDK 5.0.7 채택을 확정하지 않는다.

## 환경과 범위

- npm Emception/browser 4.5.0, 실제 Emscripten 5.0.7. 기존 SDK 6.0.13과 독립된 ignored `logs/emception/gameguild-002..004` 사용.
- [공식 데모 Next 설정](https://github.com/gameguild-gg/gameguild/blob/a4cdcfe63c07e8387e93aacbea71d50b05faa0b7/demos/emception-ide-next/next.config.ts)에 맞춘 Next 16.4.2/Turbopack, Python raw-loader, COOP same-origin/COEP credentialless. 공식 npm 진입점 `createEmception` 사용. IDE 전체 UI의 복제 검사가 아니라 같은 번들러·Worker·raw import 구성의 최소 실행 앱이다.
- [공식 테스트 설정](https://github.com/gameguild-gg/gameguild/blob/a4cdcfe63c07e8387e93aacbea71d50b05faa0b7/tools/emception/playwright.config.ts)의 5분 기준으로 compile 제한시간을 늘렸다. Firefox headless, 외부 요청 차단, loopback 공개 패키지·합성 소스만 사용했다.
- 원본 npm 패키지는 유지했다. 003은 browser 패키지 복사본에서 lld 사전 로딩에 cache-debug/cache-gl-variants/cache-misc를 추가한 진단이다. 004는 compiler.mjs 생략을 해제한 별도 진단이다. 이를 upstream 그대로의 성공으로 처리하지 않는다.

## 확인 결과

1. 문서의 `compileAndRun(source)` 기본 호출은 기존 fixture와 Next 모두 `main.cpp`를 찾지 못해 exit 1이다. 공개 API의 `sources`에 절대 경로를 주면 `HELLO_OFFICIAL_ABSOLUTE` 출력과 exit 0, STL vector/iostream은 `STL:3`과 exit 0이다. 상대 경로 문제는 번들러 변경으로 해결되지 않으며, 절대 경로 사용으로 우회 가능하다.
2. 공식 C++ preset은 Clang→wasm-ld→wasi-run 경로다. preset 자체에 `--allow-undefined`와 `--import-undefined`가 있으며 별도 runtime imports를 제공한다. 이 실행 성공은 DALi 생산 strict 링크·WebGL·pthread 성공이 아니다.
3. Next에서 빈 main을 `em++ -O2 -sERROR_ON_UNDEFINED_SYMBOLS=1 -o main.js`로 처리하면 Wasm 생성 이후 JavaScript 생성에서 exit 1이다. 589-byte Wasm은 있지만 JS는 없다. 오류는 `Did not receive forwarded data in pre output - process failed?`다.
4. 배포 tool-runner의 `runJavaScript()`는 compiler.mjs 호출을 실제로 수행하지 않고 빈 deps/asyncFuncs/extraLibraryFuncs JSON을 반환한다. 반면 배포 Emscripten의 `tools/emscripten.py`는 실제 glue와 `//FORWARDED_DATA:`를 요구한다. 실행 로그의 생략 메시지, Python assertion, 두 구현을 대조했다. 버전 문자열을 맞추거나 대기시간을 늘려 해결되는 경로가 아니다.
5. WebGL2/pthread의 lld가 각각 `libGL-webgl2-getprocaddr.a`/`libGL-mt-getprocaddr.a`를 비동기 open하는 과정에서 Wasm trap이 발생했다. 사전 로딩 보강 복사본에서는 두 검사 모두 이 정체를 넘어갔지만 결국 동일한 JavaScript 생성 assertion으로 exit 1이다. 로딩 보강은 원인 구분에 유효했으나 제품 사용 가능성을 충족하지 않는다.
6. compiler.mjs 생략만 해제하면 기존 JS 실행기는 shebang에서 파싱 실패한다. shebang만 추가 제거한 재검사도 변환된 JS 파싱에서 `expected expression, got '*'`로 실패했다. 두 검사 모두 도구 subprocess는 1인데 상위 API는 0, stderr는 emcc 오류, JS/Wasm 모두 없음으로 반환됐다. exitCode 0만으로 성공 처리할 수 없다. 기존 실행기의 Node fs/process 구현도 실제 파일 I/O 대신 stub이며 상대 ESM import/export 지원이 없다. 단순 생략 해제로 완전한 Emscripten JS 생성기가 되지 않는다.

개별 최종 상태·명령·시간 제한·stderr·산출물은 [증거](evidence/emception-5.0.7-followup/README.md)의 result 요약을 따른다. 출력이 없을 때 `readFile`의 null에 대한 fixture TypeError는 파일 부재를 뜻하며 별도 컴파일러 결함으로 집계하지 않는다.

| 경로 | 최종 결과 |
|---|---|
| 기본 공식 API, 기존 fixture / Next | 각각 exit 1, 상대 경로 입력 없음 |
| Next + 절대 경로 기본 C++ / STL | 각각 실행 exit 0, 예상 stdout 일치 |
| Next + 빈 main O2 em++ | exit 1, JS 생성 assertion, JS 없음 |
| Next + debug O0 / WebGL2 / pthread em++ | 각각 300초 제한 초과, 도구 종료 코드 없음 |
| 사전 로딩 보강 + debug / WebGL2 / pthread | 각각 exit 1, JS 생성 assertion |
| JS 생성 생략 해제 / 추가 shebang 제거 | 각각 subprocess 1, 상위 API 0, JS/Wasm 없음 — 실패 |

총 13회 브라우저 검사 수집 harness는 각각 종료0이다. 이는 위 도구/실행 결과와 구분한다. 브라우저·driver·임시 서버를 종료했고, JSON/Python 구문 검사와 `git diff --check`를 통과했다.

## 판단과 남은 범위

이번 후보를 그대로 사용하려면 필요한 일반 Emscripten JS 생성 경로가 빠져 있다. 이를 계속 구현하려면 compiler.mjs의 ESM 의존성과 Node 환경·VFS·입출력·오류 전달을 실제로 연결하고 생성 JS/Worker를 검증해야 한다. 또는 공식 그래픽 preset처럼 DALi 전용 runtime을 별도 설계해야 한다. 둘 다 단순 SDK 버전 정렬보다 큰 도구 포팅/계약 변경이므로 이번 조사에서 임의 채택하지 않았다. 빈 GL/sync, undefined 억제, 가짜 forwarded data로 성공을 만들지 않았다.

서버 없는 브라우저 컴파일 방향을 폐기하는 결과는 아니다. 일반 C++ 실행은 이번에도 확인했다. 원본 Emception 3.1.24의 기존 검사와 이 후속 fork를 구분한다. 5.0.7로 DALi를 재빌드해도 이번 DALi 없는 최소 예제의 실패는 해결되지 않으므로 재빌드를 진행하지 않았다.

Firefox 이외 브라우저·정식 IDE 전체·Pages/SW 배포·DALi 전체 링크와 GPU 실행은 이번 범위에서 검증하지 않았다. 특히 source상 JS 생성 생략과 실제 Python assertion이라는 독립적인 장애가 있으므로 다른 브라우저 성공을 가정해 채택하지 않는다. 컴파일러 재빌드·전체 Node 호환 계층 구현도 하지 않았다. 새 운영 서버 없이 로컬 검증만 했다.

기존 생산 입력 497개 hash 일치. 생산 코드·SDK·lock·P0·기존 산출물 보존, commit/push/배포 없음. 공통/native 코드 변경이 없어 GLES/Vulkan/DYNAMIC 회귀를 재실행하지 않았다. P2 생산 링크 결과와 P3/P4의 GPU 보존·격리·종료/재생성 게이트는 그대로다.
