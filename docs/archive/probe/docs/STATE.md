# DALi 브라우저 포팅 현재 상태

갱신일: 2026-10-23. P0·R0·R1 및 R2-a 완료. R2 전체는 진행 중이며 R2-b·P1 이후는 미착수입니다. 새 repo `main` commit `728cf818d4db74890a4c37d01737c8eea3d9a14b`를 원격에 push하고 SHA 일치를 확인했습니다. 현재 고정 Core 소스로 Wasm 빌드·Node 애니메이션 50/100 검증을 완료했습니다. 브라우저·GL 재검증과 기록 전환은 R2-b에 남겼습니다. 남긴 실행 프로세스는 없습니다. [R2-a 결과](R2-a-RESULTS.md), [Tizen 기준](TIZEN-BASELINE.md).

## R2-a 현재 고정 소스 검증

| 항목 | 결과 | 증거 |
|---|---|---|
| Core Debug 정적 Wasm 빌드 | 375단계 완료, 종료 0. 고정 Core SHA + 생성 헤더 경로용 CMake 옵션 | build.log (로컬 보존 자료: `webassembly-probe/logs/r2-a/run-001/build.log`; [위치 안내](../../../MIGRATION.md)) |
| Node linker-test | 종료 0 | linker-test.log (로컬 보존 자료: `webassembly-probe/logs/r2-a/run-001/linker-test.log`; [위치 안내](../../../MIGRATION.md)) |
| Node 애니메이션 | 중간 50.0, 최종 100.0, 종료 0. 모의 그래픽·단일 스레드 | core-smoke.log (로컬 보존 자료: `webassembly-probe/logs/r2-a/run-001/core-smoke.log`; [위치 안내](../../../MIGRATION.md)) |
| 원본/P0 보존·구문·ignore | 통과 | validation.log (로컬 보존 자료: `webassembly-probe/logs/r2-a/run-001/validation.log`; [위치 안내](../../../MIGRATION.md)) |
| 새 Adaptor·브라우저·GL·UI | 미검증 | R2-b 및 P6 대상 |

## 과거 P0 검증 (현재 SHA의 결과가 아님)

| 항목 | 결과와 한계 | 증거 |
|---|---|---|
| Core 전체 Debug 정적 빌드 | 소스 수정 없이 WebAssembly 빌드 성공 | 빌드 로그 (로컬 보존 자료: `webassembly-probe/logs/build.log`; [위치 안내](../../../MIGRATION.md)) |
| Core 애니메이션 | Node·Firefox에서 중간 50, 최종 100. 모의 그래픽·단일 스레드 테스트 | Node 로그 (로컬 보존 자료: `webassembly-probe/logs/core-smoke.log`; [위치 안내](../../../MIGRATION.md)), Firefox 결과 (로컬 보존 자료: `webassembly-probe/logs/browser-result.json`; [위치 안내](../../../MIGRATION.md)) |
| Adaptor 컴파일 | 주요 6개 번역 단위 성공. 전체 라이브러리 빌드 아님 | 컴파일 요약 (로컬 보존 자료: `webassembly-probe/logs/adaptor-summary.txt`; [위치 안내](../../../MIGRATION.md)) |
| GL/EGL 링크 탐색 | 265개 심볼 중 기본 7개, FULL_ES3 적용 시 3개 미해결 | 링크 결과 (로컬 보존 자료: `webassembly-probe/logs/gl-symbols.json`; [위치 안내](../../../MIGRATION.md)) |
| 실제 Canvas 렌더링 | 미구현·미검증 | P3 대상 |

GLES 3.1/3.2 헤더는 SDK에 있어 포함 자체는 컴파일 장애가 아니었습니다. 최초 의심과 실제 결과를 구분합니다.

## 작업 환경

- 코드 작업 루트: `/home/jy/work/dali/dali-web`. 기록 기준: `/home/jy/work/dali/webassembly-probe/docs`.
- SDK Emscripten 6.0.13, Node 24.19.0, CMake 3.22.1, Ninja 1.10.1, Python 3.10.12. 기존 SDK 재사용, 새 repo toolchain.lock.json·docs/BUILDING.md 참고.
- 기존 브라우저 검증: Ubuntu의 Firefox 157.0.1. Windows 미검증.
- P0 Core revision: `800960a7e226e1c6afd19b6a492b32ea7c11ed50`.
- P0 Adaptor revision: `8448c8c07bdec428eee9742eec3db2129a703772`.
- 이번 확인에서 `src/dali-core`, `src/dali-adaptor`의 tracked 변경 없음. 생성 헤더와 빌드 산출물은 존재합니다.
- 새 소스 repo: `/home/jy/work/dali/dali-web`. 다음 작업은 R2-b 브라우저·GL 재검증과 기록 기준 전환입니다. R2-b 완료 전까지 현재 복제본·SDK·로그와 기록 기준을 보존합니다.
- 빌드는 최대 `-j6`. clang-format 수동 실행 금지. R2-a commit·push 완료, Pages 배포 없음.

## 저장소 상태와 미결 사항

- 원격: https://github.com/dalihub/dali-web. R1-b 시작 시 refs 없음 재확인 후 main 생성·push. 로컬 `/home/jy/work/dali/dali-web`은 clean이며 `origin/main`과 SHA 일치.
- 세 subtree import·소스 업로드 완료. GitHub Pages 활성화는 하지 않았습니다.
- 관리 방향: 단일 repo와 subtree. 별도 포크 3개·submodule·배포 시 patch 적용 방식은 채택하지 않습니다.
- 현재 원본은 Tizen 공식 `platform/core/uifw/dali-core`, `dali-adaptor`, `dali-ui`의 `refs/heads/tizen`. 최초·후속 subtree 모두 `--squash`. [변경 결과](TIZEN-BASELINE.md).
- import 기준: Core `e2ce797a41cff49851361210491c8ed38bea46e9`, Adaptor `f0dd9b39d5a51ff0233ea50bf872dd6200e93981`, UI `f6c046d2a15ab7c6bad8d76612e667d66ed1f6d0`.
- 특이 사항: 최신 Tizen Core·Adaptor는 P0와 tree가 다릅니다. P0 표는 과거 결과입니다. 현재 Core는 R2-a에서 재검증했고 Adaptor·브라우저·GL은 R2-b에서 재검증해야 합니다. UI 결합 빌드도 미검증입니다. Tizen 원본 갱신에 SSH 접근 설정이 필요합니다.
- UI 원본의 사용자 변경 2개와 기존 P0 로그 hash 불변 확인. R1-b 검증 clone `/tmp/dali-r1b-verify-clone`과 임시 commit wrapper는 남아 있으나 후속 필수 입력이 아닙니다. 증거는 `logs/r1-a/`, `logs/r1-b/`, `logs/r1-b-tizen/`, `logs/r2-a/run-001/`.
- R2에서 현재 실험을 이관·재검증한 후 새 repo의 docs를 기록 기준으로 전환합니다.
- P1에서 Pages 호스팅 조건과 스레드 설계의 양립 여부를 검증할 방안을 정합니다.

## 남은 설계 결정

1. 웹용 최소 플랫폼 계층의 파일 경계와 생산 Adaptor로 통합하는 방법.
2. 단일 Canvas·컨텍스트와 리소스 업로드의 호출 순서.
3. 이벤트·업데이트·렌더 실행 스레드와 브라우저 프레임 루프 연결.
4. Memory2 버퍼 업로드 재사용 또는 FULL_ES3 매핑의 선택.
5. 네이티브 pixmap 경로, 고급 blend, 셰이더 바이너리의 지원 정책.

FULL_ES3로 링크되는 것과 실행 의미가 맞는 것은 다릅니다. 현재 Memory3의 WRITE 단독 매핑은 SDK의 invalidate 플래그 조건과 맞지 않습니다. 컨텍스트 공유와 생산 스레드의 대기는 아직 런타임 검증을 하지 않았습니다.

## 다음 시작점

[HANDOFF](HANDOFF.md)의 R2-b 범위를 사용합니다. R2-a 완료 후 중단했으며 다음 단계를 자동 실행하지 않습니다. 다음 컨텍스트용 요청은 [프롬프트](NEXT-CONTEXT.md)에 있습니다.

## context 세분화와 성능 상태

R1-a → R1-b → R2-a → R2-b → P1 순서로 진행합니다. 후반 단계는 PLAN의 분할 후보를 기준으로 직전 결과에 맞춰 조정합니다. R1-a·R1-b·R2-a 완료, R2-b 이후는 미착수입니다. 실제 브라우저 렌더링 성능은 미측정이며 P3부터 초기 로딩과 연속 프레임 성능을 나눠 측정합니다.
