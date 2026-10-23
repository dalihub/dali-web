# 다음 context 인수인계: P2-b sync 환경 분리부터

2026-10-23. **P2-b source/context 중간 구현, 상위 P2 미완료.** 작업 repo `dali-web`; 과거 `webassembly-probe`는 증거/SDK 보존 공간.

## 먼저 읽을 것

상위 AGENTS.md, README, [STATE](STATE.md), [PLAN](PLAN.md), [NEXT-CONTEXT](NEXT-CONTEXT.md), [P1 계약](P1-EXECUTION-CONTRACT.md), [P2-a manifest](P2-a-MANIFEST.md)·[결과](P2-a-RESULTS.md), [P2-b 중간 결과](P2-b-RESULTS.md)·[전체 순서](P2-b-HANDOFF.md), [증거](evidence/p2-b/README.md). upstreams/toolchain lock, BUILDING·UPSTREAMS·MIGRATION도 따른다.

## 현재 코드·검증

시작 clean main HEAD/원격은 `a68e3f00f0c7752561137c179500b004b930285e`. 현재 HEAD/status/remote 재확인. 이번 remote push 없음. 원본/SDK/과거 산출물/lock 변경 금지.

- Controller rename 및 공통 context의 EGL/RTTI·native callback context를 native/web 파일로 분리했다. Web은 한 surface/resource cache와 current context identity를 사용한다. host는 아직 없다.
- `DALI_GLES_ENVIRONMENT` 기본 NATIVE_EGL. native source 집합은 rename + 추출 2개 외 보존, Vulkan 집합 동일. WEB production closure는 아직 configure 거부; `cmake/p2-b`는 부분 경계 진단이다. pending source 목록을 완성된 Web closure로 오인하지 않는다.
- 최종 `logs/p2-b/context-web-003`: Core 333단계, 32개 compile/nm 각각 0. strict 기본/FULL_ES3 각각 1, 미해결 13/9개, map 없음. Controller/common Context/Web context/rendering에서 concrete EGL context 심볼 제거 확인. native sync·FileLoader/version/error·GL mapping/BlendBarrier는 남음.
- `native-001`: GLES/VULKAN/DYNAMIC configure 각각 1(host C/C++ compiler 없음). native context/rendering/egl-graphics Emscripten 교차 compile/nm 각각 0. host-native build/install/runtime과 구분.
- 기존 P2-a probe 참조도 rename/native 추출을 반영했다. 정확한 과거 source 재현은 P2-a commit 기준, 과거 evidence/hash/log/build는 그대로.

## 다음 한 가지 행동

[P2-b 순서](P2-b-HANDOFF.md) 3번 **sync 환경 분리**. `gles-sync-pool` SharedSyncObject/AgingSyncObject, texture dependency policy, Controller CreateSyncObject의 concrete EGL/FD 의존을 native로 옮긴다. `GetSyncImplementation`은 기존 추상 계약을 반환하지만 호출부 일부는 여전히 native 타입으로 cast한다. Web host에 그대로 전달하면 안 된다. WebGL2 fence/poll의 완료/실패/해제 의미를 구현한다. frame age·CPU flush·빈 성공으로 대체하지 않는다.

이후 웹 메모리/버퍼 소유 shadow → wrapper/capability → source shader/cache 경계 → 실제 host/strict 생산 링크. source manifest를 각 단계에 맞춰 갱신하고 WEB closure 거부는 실제 구현이 갖춰진 뒤 제거한다. native generated source 전체 binary-directory 정리는 아직 끝나지 않았다.

## 후속 게이트·보존

1. Core·GraphicsInterface·GraphicsSyncAbstraction 계약 변경, 전체 플랫폼 포팅, 실험 확대, 합의 방향 변경이 필요하면 중단·보고한다. native regression은 수행하거나 미검증 이유·범위를 명시.
2. 실제 Core·생산 Controller·자원·명령·host가 포함된 기본 WebGL2 strict 링크 exit 0 및 manifest/nm/map 필요. mock/빈 GL·EGL·sync/undefined 억제 금지. FULL_ES3는 runtime/보존 근거가 아님.
3. P3 픽셀/GL 오류/업로드 보존은 별도. D3 main 이벤트 + render pthread, main blocking 제거를 유지.
4. 최초 pthread/browser 단위의 로컬 헤더/SW 비교, 별도 승인 실제 Pages 최초/재방문·업데이트·하위 경로 검증. 실패/과도한 비용이면 직접 호스팅 검토, 다른 관리형 서비스 금지.
5. P3/P4 이벤트 Worker 격리 후보를 조기 검증하고 P4 종료 전 무한 루프·전체 pthread/Canvas 종료/재생성 기록. 기본 E는 임의 코드 격리 보장이 아님.
6. 원본/P0·SDK·R2/P1/P2-a 증거·기존 output 보존, 별도 build/log, 최대 -j6. upstream 갱신·Pages 배포·서버 구축 금지. 개인 SDK 경로 필수화·대형 산출물 commit 금지.

현재 미결 설계 결정은 없으며 sync부터 이어갈 수 있다. 이번 중간 인수인계를 P2-b 또는 P2 전체 완료로 간주하지 않는다.
