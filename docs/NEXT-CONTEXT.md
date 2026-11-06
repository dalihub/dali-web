# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅의 P3/P4 후속을 이어가자.
작업 repo: /home/jy/work/dali/dali-web
origin: https://github.com/dalihub/dali-web.git
2026-11-05 작업 시작 HEAD/origin main은 04a686e9ac70c6a380ff045ed635723f8861d108,
clean 확인 후 P3 GPU 메모리/실패 검사를 구현했어.
2026-11-06 코드·검사·증거와 이 인계 문서를 함께 커밋했어.
실제 HEAD/status/remote부터 확인하고 현재 변경·원본 사용자 변경을 보존해줘.

상위 AGENTS.md와 docs/P3-GPU-MEMORY-RESULTS.md,
docs/evidence/p3-gpu-memory/README.md, STATE·HANDOFF·PLAN·BUILDING,
P3-INITIAL-RESULTS·PLAYGROUND-COMPILATION·P1-EXECUTION-CONTRACT를 읽어줘.
과거 인계의 미push·최초 렌더 미검증·native compiler 부재는 당시 기록이야.

완료한 후속 단위:
- DALI_WEB_TEST_MEMORY 기본OFF, 실제 생산 Controller/Core의 GPU 검사.
- Firefox 직접 헤더/SW 각각 두 프레임115개 검사 통과, GPU buffer/uniform46개
  readback 실제값/기대값 일치, GL error0, ready→stopped.
- 초기16-byte→중간4-byte 보존, callback 반환0/4/16·무쓰기0,
  map offset3/lock offset3, Unlock(false)→Unmap, CPU/GPU uniform,
  grow/recycle·detach·마지막 참조·custom allocator 해제 확인.
- CPU uniform 중간4-byte는 실제 Program upload 후 GL uniform 조회,
  DALi scene의 uniform 색 변경은 두 프레임 픽셀로도 확인.
- reflection 중 실제 GL 오류가 만든 Wasm trap을 기존 CanSubmit gate로 수정.
  malformed shader와 실제 GL 오류 모두 ready→failure→stopped.
  공통 내부 수정이며 공개 Graphics API·native Memory2/3·native retry 유지.
- 최종 생산 build/p3/gpu-memory-final-002의35개 compile/nm·strict0,
  Core는 새 gpu-memory-final-001의333단계 archive 재사용, Core 입력 drift0.
  final-001의 host header hash drift가 있어 생산 전체 최종 증거로 쓰지 마.
  final-002 전체499개 입력 drift0. 이전 pixels-3124-001도 중간 결과야.
- production-memory-final-001은 계측 없는 host 별도 compile/strict0,
  browser-production-memory-final-001은 오류 없이 ready→stopped.
- native source GLES/VULKAN/DYNAMIC·Web closure 통과.
  현재 GNU11.4 compiler 존재, native configure3개0,
  관련14개 GNU C++ compile/nm0 및14개 Emscripten compile/nm0.
  전체 Adaptor native build3개는 설치된 Core 헤더를 찾지 못해 실패1.
  native build/install/runtime 전체 성공은 아니야.
- 기존748개 lock/산출물 hash 보존, commit/push·배포·upstream 갱신 없음.

단계 상태는 P3 진행 중이야. 위 GPU 메모리/재현 실패 단위만 완료했어.
정상 stopped를 전체 자원 회수·격리·종료/재생성 완료로 선언하지 마.
다음 작업 범위/우선순위는 사용자 요청을 확인하고 다음 게이트를 유지해줘:
1. P3/P4 이벤트 Worker 플레이그라운드 격리 조기 검사.
2. SW 재방문·업데이트·해제/실패·캐시 정책.
3. P4 종료 전 무한 루프 응답성, 전체 pthread/Canvas 종료·새 Module 재생성.
4. 실제 Pages 검증·배포는 별도 승인. GitHub Pages+SW 우선 유지.
Chromium·context loss·전역 OOM·모든 GL 실패 위치·전체 누수는 미검증이야.

승인된 원본 Emception+Emscripten3.1.24 기준을 유지해줘.
별도 운영 컴파일 서버 없이 브라우저에서 사용자 C++를 컴파일해.
후보/승인 설계 재승인은 필요 없어. toolchain.emception-3.1.24.lock.json과
scripts/env-emception-3.1.24.sh를 사용하고 기존6.0.13 SDK·lock·산출물은
보존해. 객체를 혼용하지 마. 로컬 참고는 logs/emception/alignment-3124-001/env.sh,
EM_CONFIG/EM_CACHE·Node·host 도구는 BUILDING을 따라줘.
새 build/log, EMCC_CORES=6, 빌드 최대-j6을 유지해줘.

Unlock/Unmap은 void, Core bool은 GPU 성공을 뜻하지 않아.
mock/빈 GL·undefined 억제로 성공을 만들지 마.
공통/native 회귀와 못 한 검사의 이유·영향을 명시해줘.
로컬 loopback 임시 파일 제공은 허용된 검증 범위야.
원본/P0·SDK·과거 증거·lock·산출물을 보존하고 큰 로그·프로필은 커밋하지 마.
upstream 갱신·운영 서버·Pages 배포·다른 관리형 호스팅 전환은 하지 마.
승인 범위를 넘는 공통/public 계약 변경·전체 플랫폼 포팅·합의 방향 변경이
필요하면 중단하고 보고해줘. PLAN·STATE·JOURNAL·HANDOFF·NEXT-CONTEXT를 갱신하고,
구현·특이 사항·검증·미검증·단계 상태를 구분해 보고해줘.
```
