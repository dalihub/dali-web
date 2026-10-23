# P2-b source/context 중간 결과

2026-10-23 (Asia/Seoul). **P2-b 진행 중, 상위 P2 미완료.** 사용자 요청의 전체 P2-b를 완료한 결과가 아니라, NEXT-CONTEXT의 검증 가능한 단위별 중간 인수인계다. 다음 구현은 sync 환경 분리다.

## 구현 범위

1. `EglGraphicsController`를 `GlesGraphicsController`로 정리하고 production/native/tests/probe 참조를 갱신했다. 호환 alias·중복 Controller 없음. Core public 계약·GraphicsInterface·GraphicsSyncAbstraction 변경 없음.
2. `DALI_GLES_ENVIRONMENT=NATIVE_EGL` 기본값과 WEB 검증을 추가했다. GLES/VULKAN/DYNAMIC 축은 유지하며 WEB+VULKAN/DYNAMIC, 비-Emscripten WEB, 알 수 없는 환경은 거부한다. 기존 native source 집합에 context 추출 파일 두 개만 추가됐다.
3. Controller의 concrete EGL context 조회·RTTI를 `native-gles-context.cpp`로 이동했다. `Initialize`는 기존 GraphicsSyncAbstraction 참조를 보관한다. 남은 concrete sync 사용자는 아직 native 구현이며 `GetSyncImplementation` 뒤에서 native 캐스트한다. 이 상태를 완성된 Web sync로 사용하면 안 된다.
4. Web은 등록 surface 하나와 resource 역할에 같은 `GLES::Context`를 사용한다. 실제 current WebGL handle 확인, context 교체·중복 surface 거부를 추가했다. HTML5 context 생성·소유자 스레드·종료를 구현한 host는 아직 없다.
5. `gles-context.cpp`의 native callback용 EGL 생성·복원·삭제도 별도 `NativeRendering` 내부 경계로 이동했다. native는 기존 동작을 유지하며 Web은 해당 EGL 전용 요청을 assert로 거부한다. 빈 성공 함수가 아니다.
6. source 목록은 common/native/web/pending으로 나눴다. sync·메모리·program cache 분리가 끝나지 않아 WEB production closure는 **의도적으로 configure 실패**한다. `cmake/p2-b`는 아직 native sync/memory/wrapper를 포함하는 **context 경계 진단**이다. generated namespace/cache는 진단 binary directory에 생성한다. 정식 native 생성 경로 전체 정리는 미완료다.

## 검증

최종 전체 실행은 `logs/p2-b/context-web-003`, native 검사는 `native-001`, source 검사는 `sources-002`. [검토용 증거](evidence/p2-b/README.md)에 명령·개별 종료 코드·버전·manifest/hash·nm와 실패 링크 로그를 남긴다.

| 검사 | 결과 | 판정 범위 |
|---|---|---|
| pthread Core Debug | 333단계 exit 0 | mock helper 미포함, runtime 미실행 |
| context boundary 32개 compile/nm | 각각 exit 0 | 생산 자원·명령·Controller와 link-only harness |
| Controller/common Context/Web context/rendering nm | EGL context API·EglGraphics RTTI 없음 | 전체 Web closure의 native 의존 제거 완료는 아님 |
| 기본 WebGL2 strict 링크 | exit 1, 미해결 13개 | native fence 3 + binary cache utility 5 + GL 5 |
| FULL_ES3 strict 링크 | exit 1, 미해결 9개 | 매핑 관련 GL 4개만 감소, runtime/보존 근거 아님 |
| GLES/VULKAN/DYNAMIC source 비교 | exit 0 | 기존 목록 보존, rename 및 native 추출 2개 반영, Vulkan 목록 동일 |
| 잘못된 환경 조합 4개 | 각각 예상 exit 1 | 지원되지 않는 구성 거부 |
| native GLES/VULKAN/DYNAMIC configure | 각각 exit 1 | 호스트 C/C++ compiler를 찾지 못함 |
| native context/rendering/egl-graphics 교차 compile/nm | 각각 exit 0 | Emscripten 컴파일이며 host-native 빌드/실행이 아님 |

map은 생성되지 않았다. JS/Wasm 실행 파일·실제 host·Canvas/Worker 실행 증거 없음. 모든 object를 직접 링크하고 undefined 오류를 유지했다. P2-a의 16/12개에서 EGL context/RTTI 3개만 감소했다.

최초 `context-web-001`의 Controller 컴파일은 EGL include 제거 후 드러난 전이 include 의존 두 개로 실패했다. EnvironmentVariable 헤더를 직접 포함하고 UBO alignment 조회를 기존 GL abstraction으로 경유하도록 수정했다. 002는 32개 compile/nm 통과, 003은 중복 include 정리 및 입력 hash 수집을 반영한 최종 재실행이다. 실패 결과는 보존했다.

## 특이 사항·미검증 범위

1. P2-a 미해결 목록에 없던 공통 `gles-context.cpp`의 EGL API 8개가 nm에서 확인됐다. SDK가 링크 이름을 제공한다는 사실로 Web 호환성을 승인하지 않고 native 파일로 이동했다. D1 파일 분리 방향 안의 변경이며 공통 public 계약은 변경하지 않았다.
2. native compile/install·실제 surface/context 공유·native callback·sync·shader cache·shutdown 실행은 호스트 toolchain 부재로 미검증이다. 영향 범위는 Controller rename 소비자, context 상태/수명, GL abstraction 경유 alignment 조회, native 빌드 source 선택이다. Vulkan 본체는 변경하지 않았고 일부 테스트 include만 rename했다.
3. Web 단일 cache/identity 정책은 compile/nm만 검증했다. 실제 GPU fence·메모리 보존·GL capability·source shader·host·main 비동기 ready/stop은 미구현/미검증이다. P3 픽셀 게이트, 최초 pthread/browser 로컬 헤더/SW 비교 및 별도 승인 Pages 실제 검증, P3/P4 격리 조기 게이트를 유지한다.
4. `cmake/p2-a`와 기존 probe는 rename 및 native 추출 source를 반영했다. 과거 P2-a 명령의 정확한 원본 재현 기준은 `a68e3f00f0c7752561137c179500b004b930285e`이며 과거 증거/hash/산출물은 그대로다.
5. source/context 단계에서 중간 인수인계한다. 다음 sync → 웹 메모리 → GL wrapper/capability → binary cache → 실제 host/strict 링크를 마쳐야 한다. 생산 링크 성공 전 P2 완료 선언 금지. 현재 미해결 설계 결정은 없으며 sync 구현을 이어갈 수 있다.

## 재현

SDK 선택은 [BUILDING](BUILDING.md)을 따른다. 출력은 존재하지 않는 새 경로를 사용한다. Python exit 0은 실패까지 포함한 결과 수집 성공이다.

```sh
export DALI_WEB_BUILD_DIR="$PWD/build/p2-b/new-context-run"
export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/new-context-run"
source scripts/env.sh
python3 scripts/check-gles-boundary.py

export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/new-source-run"
python3 scripts/check-gles-sources.py

export DALI_WEB_GENERATED_INCLUDE_DIR="$DALI_WEB_BUILD_DIR/generated/include"
export DALI_WEB_BUILD_DIR="$PWD/build/p2-b/new-native-run"
export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/new-native-run"
python3 scripts/check-gles-native.py
```

최대 -j6, 원본/P0·lock·기존 증거·기존 Core 산출물 보존. upstream 갱신·SDK 설치·Pages 배포·서버/브라우저 실행 없음. 로컬 중간 checkpoint만 남기고 원격 push는 하지 않는다.
