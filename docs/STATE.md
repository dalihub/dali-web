# DALi 브라우저 포팅 현재 상태

갱신일: 2026-10-23 (Asia/Seoul). **P1 설계·P2-a 진단 완료, P2-b sync 중간 구현, 전체 P2-b·상위 P2 미완료.** 현재 기록은 `dali-web/docs`. P0·R0·R1·R2 완료 유지.

작업 repo `dali-web`, main, origin `https://github.com/dalihub/dali-web.git`. 이번 시작 HEAD `7b6ee23ff36b3658fc957e1607de2d21963796c1`, clean. 실제 원격 조회 main은 `a68e3f00f0c7752561137c179500b004b930285e`. 현재 로컬 checkpoint는 git log로 확인한다. 원격 push·upstream/lock 갱신 없음.

## 현재 결과

| 대상 | 결과 | 한계 |
|---|---|---|
| 과거 context 경계 | 32개 compile/nm, strict 13/9개 미해결 | 기존 증거 보존 |
| 최종 sync-web-003 | pthread Core 333단계, 34개 compile/nm 각각 0 | link-only harness, host 없음 |
| strict 기본/FULL_ES3 | 각각 exit 1·10/6개 미해결 | map 없음, 생산 링크 미완료 |
| WebFence 수명 단위 검사 | compile/link·Node 각각 0 | GL 응답 주입, 실제 GPU 아님 |
| GLES/VULKAN/DYNAMIC source 회귀 | exit 0 | native context 2개 + sync 2개 추출, Vulkan 목록 동일 |
| native configure | 3개 각각 exit 1 | host C/C++ compiler 부재 |
| native 추출 코드 5개 | 교차 compile/nm 각각 0 | Emscripten, native build/runtime 아님 |

[중간 결과·재현](P2-b-SYNC-RESULTS.md), [증거](evidence/p2-b-sync/README.md), [전체 적용 순서](P2-b-HANDOFF.md).

## 구현·다음 단계

공통 Controller/context/native rendering 분리에 이어 SyncPool·texture dependency policy·Controller sync factory의 concrete EGL/native FD 의존을 native 파일로 이동했다. Web은 실제 fence poll 결과로만 완료를 판정하며 실패/삭제/종료를 구분한다. GL owner가 poll/delete하고 다른 스레드 해제는 owner collection으로 넘긴다. frame age·CPU flush를 GPU 완료로 처리하지 않는다. native 정책은 보존했다.

다음은 **웹 메모리와 버퍼 소유 CPU shadow**다. native Memory2/3 유지, Memory4 없음. 이어 wrapper/capability → shader cache → 실제 D3 host → 기본 WebGL2 생산 strict 링크와 source/generated 정리. host는 WebFence 실패 latch를 비동기 failure/stop에 연결해야 한다. 현재 Web host는 없다. WEB production configure 거부를 유지하고 cmake/p2-b는 부분 경계 진단으로만 사용한다.

sync의 자원/스레드 수명 변경을 검증 가능한 중간 단위로 인수인계한다. 남은 실행 범위는 취소하지 않았으며 **다음 웹 메모리 구현 진행 가능**하다. Core·GraphicsInterface·GraphicsSyncAbstraction 계약 변경, 전체 플랫폼 포팅, 실험 확대나 합의 방향 변경이 필요하면 먼저 중단·보고한다.

## 유지할 계약·게이트

1. D1 공통 GLES + Native/EGL·Web 파일 선택, Core 계약과 GLES/Vulkan/DYNAMIC 유지.
2. D2 필요한 버퍼 소유 CPU shadow·부분 갱신 보존. FULL_ES3는 데이터 보존/런타임 호환성 근거가 아니다.
3. D3 main 이벤트 + update/upload/render pthread. DOM 경계와 main ready/stop 비동기화.
4. D4 최초 pthread/browser 단위의 로컬 헤더/SW 비교, 별도 승인 Pages 실제 URL 검증. 실패/과도한 비용 시 직접 호스팅 검토. 배포·서버 구축·타사 관리형 호스팅 금지.
5. P3 실제 DALi 픽셀/GL 오류/업로드 보존 및 P3/P4 플레이그라운드 격리 조기 검증 유지. P4 종료 전 무한 루프·전체 pthread/Canvas 종료/재생성 결과 기록.
6. 특이 사항: Web shared fence는 같은 frame 재사용을 하지 않으며 native interop/cross-context wait를 명시 거부한다. native build/runtime·브라우저 GPU sync는 미검증이다.

원본 사용자 변경·P0·SDK·과거 증거·lock·기존 산출물 보존. 별도 build/log·최대 -j6. SDK·산출물·프로필·큰 로그 commit 제외. 생산 링크 성공 전 P2 완료 선언 금지. [인수인계](HANDOFF.md), [다음 요청문](NEXT-CONTEXT.md).
