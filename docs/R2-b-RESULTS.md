# R2-b 브라우저·Adaptor·GL 재검증 결과

2026-10-23 (Asia/Seoul). R2-b 및 상위 R2 완료. 기록 기준을 `dali-web/docs`로 전환했습니다. P1은 미착수입니다.

시작 HEAD는 `728cf818d4db74890a4c37d01737c8eea3d9a14b`, main clean이었고 원격 main도 동일했습니다. 현재 고정 Core `e2ce797a41cff49851361210491c8ed38bea46e9`, Adaptor `f0dd9b39d5a51ff0233ea50bf872dd6200e93981`, UI `f6c046d2a15ab7c6bad8d76612e667d66ed1f6d0`을 유지했습니다. lock·upstream 갱신은 없습니다. Core tree에는 R2-a의 생성 헤더 경로 CMake 변경만 있으며 Adaptor·UI tree는 lock과 일치합니다.

## 실행과 증거

새 로컬 실행 디렉터리는 `dali-web/logs/r2-b/run-001/`입니다. SDK는 기존 설치를 재사용했습니다. R2-a의 현재 Core 빌드 산출물을 재사용했고 JS·Wasm·HTML hash 불변을 확인했습니다. 이번에 Core 전체를 재빌드하거나 Node 검사를 반복하지 않았습니다. 그 결과는 [R2-a 기록](archive/probe/docs/R2-a-RESULTS.md)에 별도로 보존했습니다.

실제 전체 명령·종료 코드는 로컬 `commands.log`, `adaptor/commands.log`, `gl-commands.log`에 있습니다. [경로 정규화 명령 사본](evidence/r2-b/commands.txt), [도구 버전](evidence/r2-b/versions.json)은 repo에 포함합니다. 실행 재현은 [BUILDING](BUILDING.md)을 따릅니다.

| 검사 | 종료 코드 | 결과 |
|---|---|---|
| Emscripten·Node·CMake·Ninja·Python·SDK revision 조회 | 각각 0 | 6.0.13 / 24.19.0 / 3.22.1 / 1.10.1 / 3.10.12, SDK lock 일치 |
| 최초 sandbox Firefox·geckodriver 버전 조회 | 각각 1 | snap capability 제한, 브라우저 검증 결과 아님 |
| 최초 sandbox 원격 조회 | 128 | DNS 오류 |
| 최초 sandbox 브라우저 검사 | 1 | loopback socket 생성 PermissionError, Wasm 실행 이전 |
| 호스트 권한 Firefox·geckodriver 버전 조회 | 각각 0 | Firefox 157.0.1, geckodriver 0.37.1 |
| 호스트 권한 원격 main 조회 | 0 | R2-a HEAD와 일치 |
| 호스트 권한 Firefox 검사 | 0 | [passed, midpoint=50.0 endpoint=100.0](evidence/r2-b/browser-result.json) |
| Adaptor 검사 스크립트 및 6개 em++ 컴파일 | 모두 0 | 아래 파일 전부 성공, 순차 컴파일 |
| GL 탐색 Python | 0 | 결과 수집 성공. 링크 성공을 의미하지 않음 |
| WebGL 2 링크 | 1 | 265개 중 7개 미해결 |
| WebGL 2 + FULL_ES3 링크 | 1 | 265개 중 3개 미해결 |

컴파일 대상은 `gl-implementation.cpp`, `egl-implementation.cpp`, `egl-graphics-controller.cpp`, `gles2-graphics-memory.cpp`, `gles3-graphics-memory.cpp`, `combined-update-render-controller.cpp`입니다. 정확한 source/include/output 인자는 명령 사본에 있습니다. 생산 Adaptor 전체 빌드나 링크 검사가 아닙니다.

## P0 비교와 판정

[현재 GL 결과](evidence/r2-b/gl-symbols.json)와 [P0 결과](evidence/r2-b/p0-gl-symbols.json)의 JSON 내용이 일치합니다. 현재 SHA에서 새 object를 생성해 얻은 독립 실행 결과입니다. Core·Adaptor P0 tree는 현재 lock과 다르므로 P0 성공을 현재 SHA의 증거로 대신하지 않았습니다.

1. 기본 미해결: `eglCopyBuffers`, `eglCreatePixmapSurface`, `glBlendBarrier`, `glFlushMappedBufferRange`, `glGetBufferPointerv`, `glMapBufferRange`, `glUnmapBuffer`.
2. FULL_ES3 미해결: `eglCopyBuffers`, `eglCreatePixmapSurface`, `glBlendBarrier`.
3. 이 두 링크의 실패는 R2-b에서 비교할 예상 탐색 결과입니다. 심볼 수·미해결 집합·exitCode가 P0와 동일함을 확인하여 **탐색 재현 조건**을 충족했습니다. 생산 링크 성공 조건은 충족하지 않았으며 P2에 남깁니다. 예상 밖 검증 실패를 숨겨 완료 처리한 것이 아닙니다.

## 변경·보존·검증 한계

1. Adaptor 스크립트에 각 컴파일의 명령·실제 종료 코드 기록, GL 스크립트에 각 링크의 명령·종료 코드 기록을 추가했습니다. 컴파일 옵션·생산 소스·API는 변경하지 않았습니다.
2. 문서·이관 파일·미이관 자료를 [MIGRATION](MIGRATION.md)에 정리했습니다. PLAN·STATE·HANDOFF·JOURNAL·다음 프롬프트의 기준은 새 docs이며 기존 공간은 안내와 과거 증거를 보존합니다.
3. 원본 Core·Adaptor·UI와 P0 두 복제본의 HEAD·status·tracked diff, UI 사용자 파일, 기존 P0/R1/R2-a 로그 및 lock·Core 실행 산출물의 hash가 전후 동일했습니다. 로컬 `preservation-before.json`, `preservation-after.json`에 증거가 있습니다. SDK·P0 복제본을 이동하거나 삭제하지 않았습니다.
4. 특이 사항: sandbox에서 소켓·snap·DNS 제한이 발생하여 허용된 호스트 실행으로 재시도했습니다. 최초 실패와 재시도 로그를 모두 보존했습니다. SDK 신규 설치·다운로드 재검증은 하지 않았습니다.
5. Firefox 성공은 모의 그래픽·단일 스레드 Core 속성 애니메이션입니다. 실제 DALi 픽셀, 생산 GLES·전체 Adaptor 링크/실행, UI 결합, Windows·다른 브라우저, 성능, 프레임·입력·리사이즈, FULL_ES3 버퍼 의미·셰이더 바이너리·공유 컨텍스트·생산 스레드는 미검증입니다. 네이티브 build/install 회귀도 미검증입니다.
6. 생성물은 ignored `build/core/adaptor/`, `build/core/gl-symbols*`, `logs/r2-b/run-001/`에 있습니다. 브라우저 세션·드라이버·HTTP 서버는 검사 종료 때 정리됐습니다. SDK·산출물·프로필·대용량 로그는 commit하지 않습니다. 다음은 [P1 인수인계](HANDOFF.md)이며 이번 작업에서 실행하지 않습니다.

## 최종 문서·보존 검사

Shell/Python 구문, Git diff 공백 검사, 문서 상대 링크, archive hash, P0 결과 비교, 원본/P0 자료 보존, 생성물 ignore 검사를 실행했고 종료 0입니다. [검사 출력](evidence/r2-b/validation.txt). 기존 R1-a 원문 스냅샷 외부 링크 9개는 원문 보존 예외이며 위치 매핑을 문서화했습니다. 새 문서·archive에는 누락된 상대 경로가 없습니다. 외부 웹 URL의 네트워크 유효성은 이 링크 검사 범위에 포함하지 않습니다.

R2-b commit은 `Validate R2-b probes and migrate project records` 제목으로 생성합니다. commit SHA와 push/원격 SHA 확인 출력은 로컬 `logs/r2-b/run-001/`의 commit·push·remote 로그로 확인합니다. 자기 commit SHA를 문서에 내장하지 않으며 이후 작업은 실제 Git 상태를 재확인합니다.
