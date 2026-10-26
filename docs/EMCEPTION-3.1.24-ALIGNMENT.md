# 원본 Emception 3.1.24와 DALi 버전 정렬 검증

2026-10-26. 사용자 요청으로 기존 DALi를 별도 Emscripten 3.1.24 SDK에서 재빌드하고 원본 Emception으로 연결했다. **전체 생산 strict 링크, 브라우저에서 사용자 C++ 컴파일·DALi 생산 객체 링크, 생성된 모듈의 사용자 함수 실행까지 성공했다.** 별도 운영 컴파일 서버 없이 사용할 수 있는 조합의 증거를 확보했다. DALi 렌더링·GPU 보존·플레이그라운드 전체 인수 완료는 아니다. 기본 SDK lock은 여전히 6.0.13이다.

후속 결정: 이 결과를 검토한 사용자가 [3.1.24 조합 채택과 서버 없는 방향](PLAYGROUND-COMPILATION.md)을 승인했다. 아래는 당시 검사 범위이며 채택으로 미검증 항목이 완료되는 것은 아니다.

## 대상과 분리

- 원본 `jprendes/emception`의 기존 보존 배포물을 사용했다. [이전 검사](EMCEPTION-ASSESSMENT.md)의 3.1.24-git이며, 문제가 있던 [GameGuild 5.0.7 fork](EMCEPTION-5.0.7-FOLLOWUP.md)와 별개다. 브라우저 `em++ --version`을 다시 실행했다.
- 공식 emsdk release mapping의 3.1.24는 `54217a0950bb1dafe8808cc6207d378e323f9d74`. SDK archive SHA256은 `20e8e5bd745e3ad69c03bb877091d2fbb0c7db1eab309de8f185e9821aea40f4`다. 새 `tools/emsdk-3.1.24`에만 설치했다. 구형 archive는 `.tbz2`이며 `.tar.xz` 요청의 404도 보존했다.
- 로컬 실제 버전은 Emscripten 3.1.24-git (`68a9f990429e0bcfb63b1cde68bad792554350a5`), LLVM 16.0.0git. Node 24.19.0은 로컬 JS 생성 도구 실행에 사용했다. CMake 3.22.1, Ninja 1.10.1, Python 3.10.12. 브라우저는 Firefox 157.0.1/geckodriver 0.37.1이다. 릴리스 계열 정렬과 실제 연결 성공을 확인한 것이며 두 컴파일러 바이너리가 동일하다고 주장하지 않는다.
- 기존 6.0.13 SDK·lock·P0·생산 코드·산출물을 보존했다. 모든 빌드·브라우저 출력은 새 경로다. CMake 최대 -j6, 최종 SDK 내부 작업도 EMCC_CORES=6이다. 개인 절대 경로는 로컬 검증 선택값이며 제품의 필수 경로가 아니다.

## 실행 결과

| 검사 | 결과 | 범위 |
|---|---|---|
| 로컬 3.1.24 Core 전체 빌드 | 333단계, exit 0 | 기존 Core 전체 정적 라이브러리 |
| 생산 렌더러·host 35개 compile/nm | 각각 0 | 실제 Controller·자원·명령·Web backend·D3 host |
| 로컬 기본 WebGL2 strict 링크 | 0 | 실제 생산 입력, JS/Wasm/Worker/map 생성 |
| 브라우저 Emception의 같은 전체 생산 링크 | 0 | 기존 실패 조합의 버전 정렬 후 성공 |
| 브라우저 사용자 C++ 컴파일 + 전체 생산 링크 | 0 | DALi Vector2와 문자열 unordered_map을 쓰는 새 함수 |
| 생성 모듈의 사용자 함수 호출 | 결과 5 | 실제 DLL 대체나 mock 없이 Wasm 함수 실행 |
| 직접 COOP/COEP 로컬 실행 | isolation=true, sharedMemory=true, 결과 5 | 동일 브라우저 생성 산출물 |
| 헤더 없는 로컬 + SW 실행 | 최초 isolation=false → SW 제어 후 true, sharedMemory=true, 결과 5 | 최초 제어/reload 비교, Pages 실행 아님 |
| GLES/VULKAN/DYNAMIC source 회귀 | 0 | native source 선택 보존·잘못된 조합 거부 |

최종 로컬 `alignment-3124-native-002`, 브라우저 `alignment-3124-browser-001`을 기준으로 한다. browser production-link 관찰 시간 약 5.3초, production-user 약 5.7초이며 최초 도구 다운로드·입력 준비를 포함한 서비스 성능 수치가 아니다.

사용자 소스는 브라우저의 원본 Emception이 컴파일했다. `Dali::Vector2(3,4).Length()`를 문자열 키의 `std::unordered_map`에 넣고 읽어 5를 반환하는 `dali_user_probe`를 export했다. Core와 생산 객체는 배포 전에 로컬 SDK로 미리 빌드하는 라이브러리 입력이다. Python HTTP/브라우저 제어는 파일 제공·결과 수집만 담당했고 사용자 C++를 컴파일하지 않았다. DALi 파일을 제3자 페이지나 컴파일 API에 전달하지 않았다.

두 최종 링크 모두 `-pthread`, `FULL_ES3=0`, MIN/MAX_WEBGL_VERSION=2, ERROR_ON_UNDEFINED_SYMBOLS=1, ABORTING_MALLOC=0, OFFSCREENCANVAS_SUPPORT=1, PTHREAD_POOL_SIZE=1, ALLOW_BLOCKING_ON_MAIN_THREAD=0을 유지했다. mock Controller·빈 GL/EGL/sync·undefined 억제·임의 해시 심볼 구현을 추가하지 않았다. map에는 사용자 함수뿐 아니라 실제 Controller CreateBuffer/SubmitCommandBuffers, Core RenderManager, WebBufferMemory, host 시작 진입점이 남는다.

## 이전 오류와 특이 사항

이전 6.0.13 Core + 원본 3.1.24 조합에서는 Core가 `std::__2::__hash_memory`를 20회 참조했으나 구형 libc++가 제공하지 못했다. 이번 3.1.24 재빌드 Core의 해당 참조는 0이며 전체 strict 링크도 성공했다. 따라서 이 조합에서는 버전 정렬이 실제 해결책이었다. GameGuild fork의 JS 생성 생략 문제와 혼동하지 않는다.

첫 로컬 빌드 `alignment-3124-native-001`은 SDK 내부 병렬 수 제한을 설정하는 도중 config fingerprint가 바뀌어 캐시가 초기화됐고, 진행 중인 컴파일이 표준 헤더를 찾지 못해 실패했다. 검사 준비 과정의 실수다. 설정을 고정한 뒤 새 build/log에서 처음부터 빌드했고 소스 수정 없이 통과했다. 첫 실패·다운로드 404·재시도 기록은 유지한다.

## 완료 범위와 남은 게이트

**3.1.24 버전 정렬·브라우저 컴파일/전체 생산 링크·사용자 함수 실행 검증 완료.** 5.0.7 후보는 계속 미채택이며 원본 3.1.24 결과로 그 fork를 통과 처리하지 않는다. 저장소 기본 SDK를 3.1.24로 변경하거나 장기 제품 도구로 최종 채택한 것은 아니다.

생성 모듈의 pthread pool 초기화와 공유 메모리는 확인했지만 `dali_web_start`를 호출하지 않았다. 사용자 함수는 main에서 실행하며 DALi update/upload/render pthread, 실제 Canvas WebGL context·픽셀·GPU uniform/메모리 보존·fence shutdown·context 폐기·실패 후 새 인스턴스 재생성은 이번에 검증하지 않았다. 3.1.24에서 기존 CPU18/5모드도 재실행하지 않았다. native build/install/runtime은 기존 host compiler 부재 조건이 유지되며, 이번 생산 소스 변경이 없어 재시도하지 않았다. 전체 Adaptor/UI·범용 사용자 앱 호환성도 미검증이다.

로컬 SW 비교는 이번 모듈 로딩·함수 호출에 한정한다. Worker GL context·실제 공유 카운터 작업, 재방문/업데이트/unregister/캐시 실패, Chromium, 실제 GitHub Pages는 남는다. Pages는 별도 승인이다. P3 GPU/readback·최초 pthread 렌더 호스팅 검사, P3/P4 이벤트 Worker 격리, P4 무한 루프 응답성·전체 pthread/Canvas 종료·재생성 게이트를 유지한다. 별도 운영 컴파일 서버나 다른 관리형 호스팅으로 방향을 변경하지 않았다.

생산 입력497개 hash 변화 없음. 브라우저·driver·임시 서버 종료. 증거 JSON/Python 구문 검사와 git diff --check 통과. commit/push·upstream 갱신·Pages 배포 없음.

[명령·버전·manifest·nm·map·입력/산출물 hash·개별 결과](evidence/emception-3.1.24-alignment/README.md).
