# DALi 브라우저 포팅 현재 상태

갱신일: 2026-10-23 (Asia/Seoul). **P1 설계·P2-a 진단 완료, P2-b source/context 중간 구현, 상위 P2 미완료.** 현재 기록은 `dali-web/docs`. P0·R0·R1·R2 완료 유지.

작업 repo `dali-web`, main, origin `https://github.com/dalihub/dali-web.git`. 이번 시작 HEAD와 원격 main은 `a68e3f00f0c7752561137c179500b004b930285e`, clean이었다. 현재 HEAD/status는 재확인한다. upstream/toolchain lock 변경 없음. 이번 원격 push 없음.

## 현재 결과

| 대상 | 결과 | 한계 |
|---|---|---|
| P2-a 과거 진단 | 30개 compile/nm 통과, strict 기본/FULL_ES3 exit 1·16/12개 미해결 | 증거 보존 |
| P2-b pthread Core | 새 Debug 333단계 exit 0 | runtime 없음 |
| P2-b context 경계 | 32개 compile/nm 각각 0 | 실제 host 없는 link-only harness |
| P2-b strict 링크 | 기본/FULL_ES3 exit 1·13/9개 미해결 | map 없음, 생산 링크 미완료 |
| native source 회귀 | GLES/VULKAN/DYNAMIC 기존 집합 보존 + context 추출 2개 | Vulkan 본체 변경 없음 |
| native configure | GLES/VULKAN/DYNAMIC 각각 1 | host C/C++ compiler 부재, build/install/runtime 미검증 |
| native context/rendering/egl-graphics | 교차 compile/nm 각각 0 | Emscripten이며 native 실행 증거 아님 |

[중간 결과·재현](P2-b-RESULTS.md), [증거](evidence/p2-b/README.md), [전체 적용 순서](P2-b-HANDOFF.md).

## 구현·다음 단계

Controller를 GlesGraphicsController로 rename하고 EGL context/RTTI 및 native rendering callback을 환경별 파일로 이동했다. Web 단일 context/cache 경계만 추가했으며 context 생성 host는 없다. 기존 native sync·Memory2/3·wrapper·binary cache는 아직 진단 closure에 남아 있다. `DALI_GLES_ENVIRONMENT` 기본은 NATIVE_EGL; 정식 WEB closure와 전체 Adaptor Web profile은 미완성이므로 configure에서 거부한다. `cmake/p2-b`만 부분 경계 검사용이다.

다음은 **SyncPool·texture dependency·Controller sync 생성의 native/Web 파일 분리**다. 이어 웹 메모리 → GL wrapper/capability → shader cache → 실제 host/strict 링크. 공통 public 계약 변경 없이 진행 가능하다고 판단하며, 변경이 필요하거나 플랫폼 전체 포팅/실험 확대로 이어지면 중단·보고한다. 이번은 검증 가능한 context 단위의 중간 인수인계이며 전체 P2-b 완료가 아니다.

## 유지할 계약·게이트

1. D1 공통 GLES + Native/EGL·Web 파일 선택, Core 계약과 GLES/Vulkan/DYNAMIC 유지.
2. D2 웹 메모리 + 필요한 버퍼 소유 CPU shadow. native Memory2/3 유지, Memory4 없음. FULL_ES3 링크 지원은 보존 근거가 아니다.
3. D3 main 이벤트 + update/upload/render pthread. DOM 경계와 main ready/stop 비동기화. 아직 host·Worker runtime 없음.
4. D4 최초 pthread/browser 단위의 로컬 헤더/SW 비교, 별도 승인 Pages 실제 URL 검증. 실패/과도한 비용 시 직접 호스팅 검토. 배포·서버 구축·다른 관리형 서비스 금지.
5. P3 실제 DALi 픽셀/GL 오류/업로드 보존, P3/P4 격리 조기 검증 및 P4 종료 전 무한 루프·전체 pthread/Canvas 종료/재생성은 별도 게이트.
6. 특이 사항: P2-a 오류 목록에 없던 공통 Context의 EGL API 8개도 native로 격리. native 동작 회귀는 compiler 부재로 미검증이며 source/교차 compile 성공과 구분한다.

원본 사용자 변경·P0·SDK·과거 증거·lock·기존 산출물 보존. 별도 build/log·최대 -j6. SDK·산출물·프로필·대용량 로그 commit 제외. 실제 픽셀·Pages·전체 Adaptor/UI·성능·Windows·context loss 미검증. **생산 링크 성공 전 P2 완료 선언 금지.** [인수인계](HANDOFF.md), [다음 요청문](NEXT-CONTEXT.md).
