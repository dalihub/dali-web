# DALi 브라우저 포팅 현재 상태

갱신일: 2026-10-23 (Asia/Seoul). **P1 설계 완료, P2-a 진단 완료, P2-b 미착수, 상위 P2 미완료.** 현재 기록은 `dali-web/docs`이며 P0·R0·R1·R2는 완료 상태를 유지한다.

작업 repo `dali-web`, branch main, origin `https://github.com/dalihub/dali-web.git`. P2-a 시작 HEAD와 원격 main은 P1 완료 commit `3eb2c89ecad10f810d2ff064f2b4f3d43a42a426`, 작업 트리 clean이었다. 현재 HEAD/status는 매 작업 시작 시 다시 확인한다. upstream/toolchain lock 변경 없음.

## 현재 검증 결과

| 대상 | 결과 | 판정 범위 |
|---|---|---|
| R2 Core Wasm·Node·Firefox | mock 애니메이션 50/100 통과 | 생산 픽셀 아님, 이번 재실행 없음 |
| R2 Adaptor/GL 탐색 | 6개 compile 통과, 265 심볼 링크 기본 7/FULL_ES3 3 미해결 | 과거 비교 결과, 이번 manifest와 다름 |
| P1 SDK map guard | WRITE-only 0으로 거부 | 실제 GL 실행 아님 |
| P2-a Core | 새 pthread Debug 정적 빌드 333단계, exit 0 | mock helper 미포함, runtime 미실행 |
| P2-a GLES/utility/harness | 30개 번역 단위 각각 compile/nm exit 0 | 생산 GLES 23개 전체 포함 |
| P2-a strict 링크 | 기본/FULL_ES3 각각 exit 1, 16/12개 미해결 | 실패, map 미생성, 생산 링크 미완료 |

[상세 결과](P2-a-RESULTS.md), [manifest/재현](P2-a-MANIFEST.md), [증거](evidence/p2-a/README.md), [P2-b 계획](P2-b-HANDOFF.md).

## 설계·보존·특이 사항

1. D1 공통 GLES + Native/EGL·Web 파일 선택, Core 계약 및 GLES/Vulkan/DYNAMIC 유지. D2 웹 메모리 + 필요한 버퍼 소유 CPU shadow, native Memory2/3 유지·Memory4 없음. D3 main 이벤트 + update/upload/render pthread Worker, DOM 경계 및 main blocking 제거. D4 Pages+SW 우선, 실패/과도한 비용 시 직접 호스팅 검토·다른 관리형 서비스 금지. [P1 계약](P1-EXECUTION-CONTRACT.md).
2. 실제 링크에서 EglGraphics RTTI/context, native fence FD, shader cache의 FileLoader/version/error 의존을 확인했다. Controller·sync·shadow 분리 구현은 수행하지 않았다. FULL_ES3의 매핑 4개 해소는 runtime/데이터 보존 근거가 아니다.
3. 특이 사항: native 생성 소스 shader-cache-path를 binary directory에 생성하도록 진단 CMake만 조정. 최초 configure 실패 보존. 초기 harness에서 GC된 native 경로를 명시 참조해 10/6 → 16/12 의존을 확인. 모두 링크 전용이며 host·Canvas 실행 없음.
4. 원본/P0 HEAD·status·diff, 사용자 파일, 과거 P0/R2/P1 로그·증거, lock·기존 Core 산출물 보존 검사. 생산 `libs/` diff 없음. SDK 재사용, upstream 갱신·Pages 배포·서버 구축 없음. 빌드 최대 -j6, 새 프로세스는 종료.
5. native build/install·GLES/Vulkan/DYNAMIC 회귀, 실제 픽셀·업로드 보존·Worker/SAB/OffscreenCanvas·Pages·전체 Adaptor/UI·성능·Windows·context loss 미검증. SDK·산출물·프로필·대용량 로그는 commit 제외.

**P2-b 진행 가능**하나 이번에는 구현하지 않고 멈춘다. 미결 설계 결정은 없으며 구체적인 구현 순서와 회귀 게이트는 [HANDOFF](HANDOFF.md)를 따른다. 생산 링크 성공 전 상위 P2 완료 선언 금지. [다음 요청문](NEXT-CONTEXT.md).
