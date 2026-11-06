# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅 P3/P4 후속을 이어가자.
repo /home/jy/work/dali/dali-web, origin https://github.com/dalihub/dali-web.git.
기준 HEAD c98c04796b0b5ebe3c0029a6e37260bd13d6ffb6.
11월6일 최초 HEAD와 실제 origin/main 일치·clean 확인 후,
dedicated Worker 실패 검사와 사용자 승인 후 app 이벤트 pthread C 구현을
진행했고, 사용자 요청으로 테스트/host/CMake/문서/증거를 함께 커밋한다.
이 문서가 포함된 커밋이 후속 기준이다. 커밋 전 미커밋 설명은 당시 기록이다.
시작 전에 실제 HEAD/status/origin main을 확인해 push 여부와 clean 상태를
판정하고 사용자 변경을 보존해줘. 최종 채팅 인계의 커밋 SHA도 대조해줘.

상위 AGENTS.md, docs/P3-EVENT-PTHREAD-RESULTS.md와
 docs/evidence/p3-event-pthread/README.md, STATE·HANDOFF·PLAN·BUILDING,
P1-EXECUTION-CONTRACT·PLAYGROUND-COMPILATION,
P3-GPU-MEMORY-RESULTS·evidence/p3-gpu-memory/README·P3-INITIAL-RESULTS,
HANDOFF가 지정한 필수 문서를 읽어줘.
P3-EVENT-WORKER-RESULTS는 이전 dedicated Worker 실패와 중단 기록이야.
사용자가 app 이벤트 pthread + window 종료 감독 구현을 승인했고 완료했으니
이전 중단 상태를 현재 승인 대기로 취급하지 마.

완료한 C 조기 검사:
- 기본 E 유지. 기본OFF DALI_WEB_TEST_EVENT_PTHREAD 독립 target.
- window runtime/pthread pool → event pthread(Core/TLS) → render pthread.
  SDK 공식 Canvas attribute 전달로 기존 실패 해소. SDK/registry 직접 수정,
  PROXY_TO_PTHREAD·main proxied GL·빈 GL/undefined 억제 없음.
- main rAF timestamp·input/trigger/stop mailbox, event 소유권/TLS assert,
  실제 KeyEventSignal keyCode65, timer/Actor 조작.
- 정상 종료의 Core/TLS→render context→pthread reap/pool 종료 확인.
- 초기화 ready 전과 실제 timer callback의 C++ 무한 루프 동안 main
  heartbeat600ms에29~30회, render counter 지속, 중단 버튼 handler 실행.
- 소속2개 pthread 모두 terminate 후350ms counter 불변·Canvas 제거,
  낡은 메시지 차단, 새 Module/shared memory/Canvas 실제 두 프레임 정상 렌더.
- 강제 종료는 allocator/free/소멸자 실행을 시도하지 않고 Module 전체 폐기.
  GPU 메모리 즉시 회수·GC·장기 누수 없음을 증명한 것은 아니야.
- Firefox 직접 헤더/SW 각각 정상→init-loop→callback-loop→새 정상4회 통과.
  synthetic DOM keydown/button click 사용. 추가 동적 pthread는 미검증.
- 새 전체 CMake371단계 configure/build0:
  build/p3/event-pthread-cmake-001, C는 그 아래 event-probe.
  최종 logs/p3/event-pthread-final-{headers,sw}-001.
- E 픽셀 회귀 event-pthread-e-regression-001 통과.
  계측 없는 E compile/nm/link/browser는 event-pthread-e-production-002,
  event-pthread-e-production-browser-002 통과.
- 이전 EM_JS export wrapper 누락/계측 없는 E의 nm 도구 선택 오류는
  중간 실패 로그로 보존. 성공 집계에 넣지 않았어.
- 기존748개 lock/산출물 drift0. 이전499개 생산 입력 중 host/CMake만
  승인된 변경. 공통 Core/Adaptor·SDK·lock·public API 변경 없음.
  native GLES/VULKAN/DYNAMIC source·Web closure 통과.
  native 전체 build/runtime 재실행 안 함, 기존 Core 설치 헤더 부재 한계 유지.

C 조기 검사 단위 완료, P3 진행 중. C 전체 지원/P4/P8/P9 완료 아님.
다음 범위는 사용자 지시를 확인해줘:
1. SW 재방문·업데이트·해제·실패·캐시 정책.
2. P4 시간/입력/resize·숨김/재개·반복/실패 수명 확대.
3. Chromium·추가 pthread·전체 Adaptor 서비스·context loss/OOM·장기 누수.
실제 Pages 검증·배포는 별도 승인, GitHub Pages+SW 우선 유지.
C supervisor를 범용 앱 인터페이스로 일반화하는 작업은 아직 하지 않았어.

원본 Emception+Emscripten3.1.24 서버 없는 브라우저 C++ 컴파일 유지.
toolchain.emception-3.1.24.lock.json·scripts/env-emception-3.1.24.sh,
로컬 logs/emception/alignment-3124-001/env.sh 및 BUILDING을 따라줘.
새 build/log, EMCC_CORES=6, 최대-j6. 기존6.0.13 SDK·lock·산출물·P0·
원본·과거 증거 보존. SDK 교체/공통/public 계약 변경·전체 플랫폼 포팅·
합의 방향 변경이 필요하면 중단·보고. upstream 갱신·운영 서버·배포·
다른 관리형 호스팅 전환 금지, 로컬 loopback 파일 제공은 허용 범위야.
PLAN·STATE·JOURNAL·HANDOFF·NEXT-CONTEXT를 갱신하고,
구현·특이 사항·검증·미검증·단계 상태를 구분해서 보고해줘.
```
