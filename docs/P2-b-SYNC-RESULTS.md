# P2-b sync 환경 분리 중간 결과

2026-10-23 (Asia/Seoul). **sync 구현·컴파일·수명 단위 검사까지의 중간 인수인계이며, 전체 P2-b와 상위 P2는 미완료다.** 실제 host와 기본 WebGL2 생산 strict 링크는 아직 없다. 이전 [source/context 결과](P2-b-RESULTS.md)와 증거는 보존했다.

## 구현

1. `SyncPool`의 shared/aging fence 구현을 `native-gles-sync-pool.cpp`와 `web-gles-sync-pool.cpp`로 분리했다. 공통 헤더의 concrete EGL union을 cpp 내부 Impl로 이동했다. native FD·EGL cast와 기존 native age/wait 정책은 native 파일에 보존했다.
2. texture dependency policy와 Controller sync factory를 환경별 `native-gles-sync.cpp` / `web-gles-sync.cpp`로 이동했다. resource factory helper를 내부 헤더로 추출해 기존 allocation/recycle/deleter 경로를 재사용한다. Core·GraphicsInterface·GraphicsSyncAbstraction 계약 변경 없음.
3. Web `WebFence`는 `glFenceSync` → flush(제출만) → zero-timeout `glClientWaitSync`로 진행한다. timeout은 pending, 두 signalled 결과만 complete, 생성 실패·WAIT_FAILED·context loss는 failed, 해제·종료는 cancelled다. terminal fence는 한 번만 삭제하고 frame age로 완료시키지 않는다.
4. 생성한 GL 소유자가 poll/delete한다. 다른 스레드는 게시된 완료 상태를 읽거나 해제를 요청할 수 있고, owner의 collection이 실제 삭제한다. pool frame 처리와 Controller/surface 종료에 collection/cancellation을 연결했다. 종료 시 context를 해제하기 전에 취소해야 하며 current context/owner를 검사한다.
5. Web shared fence는 같은 frame이어도 재사용하지 않는다. 앞서 삽입한 fence가 이후 GPU 제출까지 포함하지 않기 때문이다. native FD/EGL sync 및 cross-context GPU wait 요청은 명시적으로 거부한다. 단일 Web context의 texture dependency는 동일한 cache identity/명령 순서를 사용하며 이를 CPU-visible GPU 완료로 보고하지 않는다.
6. `HasFailure()`는 GL owner thread에 실패를 latch한다. 다음 실제 host는 이 상태를 확인해 비동기 failure/stop으로 연결해야 한다. Core의 `IsSynced()` bool은 실패를 완료로 바꾸지 않는다. 자동 복구/동일 Worker 재사용은 이번에 제공하지 않는다.
7. 정식 native source 목록과 P2-a 진단 목록에 추출 파일을 추가했다. `cmake/p2-b`는 Web sync를 선택하고 EGL sync object를 제외한다. Memory2/3·native wrapper·binary cache는 아직 남아 있으므로 정식 WEB production configure 거부를 유지한다. generated source 전체 정리는 미완료다.

## 검증

최종 `logs/p2-b/sync-web-003`, `fence-unit-002`, `sync-sources-001`, `sync-native-002`. [작은 증거](evidence/p2-b-sync/README.md)에 명령·버전·개별 종료 코드·manifest/hash·nm·실패 링크를 기록한다. 001 결과도 보존했다.

| 검사 | 결과 | 한계 |
|---|---|---|
| pthread Core Debug | 새 333단계 exit 0 | 실제 Worker 실행 아님 |
| Web sync 경계 | 34개 compile/nm 각각 0 | link-only harness, host 없음 |
| 공통/Web sync nm | concrete EGL/NativeFence/FD 의존 없음 | native wrapper의 eglGetProcAddress는 아직 남음 |
| 기본 WebGL2 strict 링크 | exit 1, 미해결 10개 | binary cache utility 5 + GL 5, map 없음 |
| FULL_ES3 strict 링크 | exit 1, 미해결 6개 | binary cache utility 5 + BlendBarrier, runtime/보존 근거 아님 |
| WebFence 주입형 단위 검사 | compile/link 및 Node 각각 0 | 실제 WebFence 코드 + 주입 GL 응답; 실제 GPU/브라우저 아님 |
| GLES/VULKAN/DYNAMIC source 회귀 | exit 0 | native 기존 집합 + context 2개 + sync 2개 추출, Vulkan 동일 |
| 잘못된 환경 조합 4개 | 각각 예상 exit 1 | configure 거부 유지 |
| native configure 3개 | 각각 exit 1 | host C/C++ compiler 부재 |
| native context/rendering/sync/sync-pool/egl-graphics | 5개 교차 compile/nm 각각 0 | Emscripten, native build/runtime 성공 아님 |

주입형 테스트는 empty/double release, 10회 collection 후에도 pending 유지, 두 완료 결과, 생성 실패, WAIT_FAILED, 다른 thread의 poll/해제 요청과 owner 삭제, 종료 cancellation, fence 교체, context loss를 검사한다. 테스트 전용 `--wrap`은 단위 테스트 스크립트에만 있으며 생산 진단 링크에는 사용하지 않는다. mock Controller나 GL stub으로 생산 성공을 판정하지 않았다.

## 특이 사항·미검증 범위

1. 같은 frame fence 재사용과 frame age 완료는 Web에 이식하지 않았다. native 의미는 그대로 보존했다. Core bool 계약을 바꾸지 않고 내부 실패 상태를 분리했으나 실제 host의 비동기 오류 연결은 후속 작업이다.
2. native configure/build/install·GL surface/resource/sync/shader cache/shutdown runtime은 미검증이다. 영향 범위는 공통 factory helper 추출, pool 내부 저장소 및 환경 policy/factory 선택, Controller 종료 hook이다. native AgingSyncObject의 Impl 분리는 fence당 추가 heap allocation을 도입하며 그 성능 영향은 측정하지 않았다. Vulkan 구현 본체·Core 계약은 변경하지 않았다.
3. 실제 browser fence/Worker GL·host frame 순서·픽셀·GPU 업로드 보존은 미검증이다. 이번 Node pthread 테스트는 GL 응답 주입 테스트로, 최초 pthread/browser 호스팅 게이트를 통과한 결과가 아니다.
4. 원본/P0 repo 상태, 과거 보존 snapshot의 1,370개 파일과 P2-a snapshot의 176개 파일, 이번 시작 lock/증거 36개 파일 hash가 일치했다(대상 간 중복 포함). SDK 설치/수정·upstream 갱신·서버·브라우저·Pages 배포 없음. 기존 build/log를 덮어쓰지 않았다.
5. sync 단위 변경이 자원 생성·삭제·스레드 간 수명에 걸쳐 있어 검증 가능한 이 지점에서 인수인계한다. **다음은 웹 메모리이며 합의된 방향으로 진행 가능하다.** 이어 wrapper/capability → shader cache → 실제 D3 host → 기본 WebGL2 생산 strict 링크 및 source/generated 정리를 마쳐야 한다. 생산 링크 성공 전 P2 완료 선언 금지.

## 재현

[BUILDING](BUILDING.md)의 SDK/host 도구 선택 후 새 경로를 사용한다. 최대 -j6. boundary Python exit 0은 결과 수집 성공이며 내부 링크 exit 1과 구분한다.

```sh
export DALI_WEB_BUILD_DIR="$PWD/build/p2-b/new-sync-run"
export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/new-sync-run"
source scripts/env.sh
python3 scripts/check-gles-boundary.py

export DALI_WEB_CORE_BUILD_DIR="$DALI_WEB_BUILD_DIR"
export DALI_WEB_BUILD_DIR="$PWD/build/p2-b/new-fence-unit"
export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/new-fence-unit"
python3 scripts/check-web-fence.py

export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/new-source-run"
python3 scripts/check-gles-sources.py

export DALI_WEB_GENERATED_INCLUDE_DIR="$DALI_WEB_CORE_BUILD_DIR/generated/include"
export DALI_WEB_BUILD_DIR="$PWD/build/p2-b/new-native-run"
export DALI_WEB_RUN_DIR="$PWD/logs/p2-b/new-native-run"
python3 scripts/check-gles-native.py
```

P3 픽셀/보존 게이트, 최초 pthread/browser 단위의 로컬 헤더/SW 비교와 별도 승인 Pages 실제 검증, P3/P4 플레이그라운드 격리 조기 게이트를 유지한다. 다른 관리형 호스팅은 금지한다.
