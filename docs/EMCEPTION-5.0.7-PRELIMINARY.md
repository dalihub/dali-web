# Emception 5.0.7 후보 예비 검증

후속 사용자 요청으로 [공식 구성·원인 구분 추가 검사](EMCEPTION-5.0.7-FOLLOWUP.md)를 수행했다. 아래는 당시 예비 검사 기록이며 최신 판정은 후속 문서를 따른다.

2026-10-26. 범위는 공개 합성 C++의 브라우저 컴파일·링크 예비 검사이며 DALi 재빌드나 SDK 전환은 포함하지 않는다. **예비 게이트 미통과**: 버전과 직접 Clang 객체 생성은 확인했지만 `em++` 전체 링크를 완료하지 못했다.

## 대상과 환경

- GameGuild `emception`, `@gameguild/emception-browser`, `@gameguild/emception-xterm` npm 패키지 4.5.0. 패키지 버전과 Emscripten 버전은 다르다.
- [공개 manifest](https://cdn.jsdelivr.net/npm/@gameguild/emception-toolchain@4.5.0/cdn/manifest.json): emsdk 5.0.7, LLVM 23.0.0git, Binaryen 129, Python 3.13.3. 로컬 core 패키지 CDN manifest와 일치한다.
- [검토한 소스 lock](https://github.com/gameguild-gg/gameguild/blob/a4cdcfe63c07e8387e93aacbea71d50b05faa0b7/tools/emception/toolchain/toolchain.lock.json). 배포 컴파일러의 실제 버전 출력은 별도로 수집했다.
- Firefox 157.0.1 headless, geckodriver 0.37.1. loopback 정적 fixture에 COOP/COEP 적용, Firefox 외부 통신은 프록시로 차단했다. 사용자 입력 컴파일은 브라우저 Worker의 Wasm 도구에서 수행했다. Python은 정적 파일 제공과 브라우저 제어만 담당했다.
- npm 배포물을 다운로드·추출하고 bare import, extensionless Worker URL, Python `?raw` import를 로컬에서 해석했다. 이는 번들러 역할을 대신하는 fixture이며 upstream의 정식 앱 빌드와 동일하다고 주장하지 않는다. 컴파일러 로직은 수정하지 않았다.
- Clang/Python/lld의 glue 및 Wasm 6개 hash가 manifest와 일치한다.

## 결과

| 검사 | 결과 | 판정 |
|---|---|---|
| browser-001 초기 API 호출 | argv[0] 누락으로 no input files, 반환 exitCode 0 | harness 오류, 성공 제외 |
| browser-002 `em++ --version` | exitCode 0, 실제 5.0.7 | 버전 확인 |
| browser-002 STL C++ | Wasm index out of bounds 이후 미완료, probe 중단 | 실패; 컴파일러 종료 코드 없음 |
| browser-003 `clang++ --version` | exitCode 0, 23.0.0git | 버전 확인 |
| browser-003 최소 함수 객체 생성 | exitCode 0, 471-byte Wasm 객체와 magic 확인 | 객체 생성 성공 |
| browser-004 빈 main 전체 링크 | Clang subprocess 0, wasm-ld 실행 후 45초 제한 초과 | 미완료; 링크 종료 코드 없음 |
| pthread / WebGL2 | 선행 전체 링크 미완료로 미실행 | 미검증 |

browser-003 소스는 `extern "C" int add(int a,int b){return a+b;}`이며 명령은 `clang++ -c /home/user/tiny.cpp -o /home/user/tiny.o`이다. browser-004는 `int main(){return 0;}`를 `em++ /home/user/minimal.cpp -O0 -sERROR_ON_UNDEFINED_SYMBOLS=1 -o /home/user/minimal.js`로 처리했다. API 호출에는 별도로 도구 이름과 argv[0]을 모두 제공했다.

browser-004 마지막 활동은 wasm-ld의 `libstubs-debug.a` 비동기 open과 Python의 subprocess 응답 대기다. 이는 관찰된 정체 위치이며 교착이나 파일 결함의 확정 원인은 아니다. 시간 제한 뒤 Worker를 dispose하고 브라우저·driver·임시 서버를 종료했다. 링크 산출물의 유효성이나 실행 성공은 확인하지 않았다.

## 특이 사항과 다음 판단

직접 Clang은 동작하므로 브라우저 C++ 컴파일 자체가 불가능하다는 결과가 아니다. 기존 3.1.24와 DALi 6.0.13 사이의 `__hash_memory` 미해결과 달리, 이번 검사는 DALi 입력 없이 컴파일 도구 실행 과정에서 막혔다. 원인을 버전 불일치나 DALi 코드로 단정할 수 없다. Firefox와 이 fixture에서의 결과이며 다른 브라우저나 upstream 정식 앱 전체의 결함 판정으로 확대하지 않는다.

5.0.7 채택과 DALi 재빌드는 아직 진행하지 않는다. 다음 검토 후보는 정식 번들러 환경과 runner/VFS 비동기 연결의 차이 확인이다. 도구 수정·추가 SDK 구축은 이번 예비 검증 범위 밖이다.

기존 생산 입력 497개 hash 변화 없음. 원본 SDK 6.0.13, lock, DALi 소스와 기존 산출물을 보존했다. 공통/native 코드를 변경하지 않아 GLES/Vulkan/DYNAMIC 회귀는 재실행하지 않았다. DALi 전체 링크, pthread/WebGL 실행, GPU 보존, Pages/SW 실행은 이번 검사로 검증되지 않는다. P3/P4 상태를 변경하지 않는다. commit/push/배포 없음.

[검사 증거](evidence/emception-5.0.7/README.md).
