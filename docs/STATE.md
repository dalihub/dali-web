# DALi 브라우저 포팅 현재 상태

2026-11-06 커밋 인계: 사용자 요청으로 C 격리 검사 코드·작은 증거·문서를 함께 커밋한다. 아래 “미커밋/commit·push 없음”은 각 검사 종료 시점 기록이다. 재개 시 이 문서를 포함한 실제 HEAD/status와 origin/main을 확인하며 최종 채팅 인계의 push 확인 결과를 우선한다.


2026-11-06 후속 구현: 사용자가 app 이벤트 pthread + window 종료 감독 방식의 구현을 승인했다. [C 조기 검사 결과](P3-EVENT-PTHREAD-RESULTS.md), [증거](evidence/p3-event-pthread/README.md). 기본OFF 독립 C target을 추가하고 기본 E를 유지했다. 새 전체 CMake371단계 빌드0, Firefox 직접 헤더/SW 각각 정상→초기화 무한 루프→timer callback 무한 루프→새 정상 실행4회 행렬 통과. Core/TLS·입력/key signal·timer/trigger 소유권, 실제 픽셀, 무한 루프 중 main heartbeat, 소속2개 pthread counter 정지·Canvas 제거·새 Module/Canvas 렌더를 확인했다. E 픽셀 및 계측 없는 E 정상 종료 회귀도 통과. **C 조기 검사 단위 완료·P3 진행 중.** 강제 종료의 C++/GL 정상 소멸, GPU 메모리 즉시 회수·장기 누수·전체 Adaptor·Chromium 지원까지 선언하지 않는다. 이전 dedicated Worker 중단은 아래 역사 기록이며 현재 승인 대기가 아니다. 다음은 SW 후속 정책/P4 남은 게이트다. 기존748개 보존 drift0, 공통/public·SDK·lock 변경 없음. commit/push·배포 없음.


2026-11-06 최신: [P3/P4 이벤트 Worker 시작 검사](P3-EVENT-WORKER-RESULTS.md), [증거](evidence/p3-event-worker/README.md). 시작 HEAD와 실제 origin/main은 `c98c04796b0b5ebe3c0029a6e37260bd13d6ffb6`, clean이었다. 아래 과거 미커밋 설명보다 이 확인을 우선한다. 기본 E는 동일3.1.24 생산 산출물로 ready→stopped 재통과. dedicated Worker를 event/main-runtime owner로 삼는 C 시작 후보는 pthread/Canvas 전달 실패(exit1), Core 생성 전에 중단했다. **P3 진행 중·C 격리 게이트 미통과.** TLS/이벤트 소유권은 정적 감사만 수행했고 무한 루프 main 응답성·전체 pthread/Canvas 회수·새 인스턴스 재생성은 미검증이다. P1 중단 조건에 따라 D3 확장 방향 결정 전 우회 구현을 하지 않는다. app event pthread + window 종료 감독 방식과 SDK Canvas adapter 대안은 결과 문서의 제안이며 승인/작업큐 등록이 아니다. 이번 변경은 테스트 fixture·문서·작은 증거뿐이며 commit/push·배포 없음. 보존748개·생산499개·runtime3개 hash drift0.


2026-11-06 인계: P3 GPU 메모리 코드·검사·증거와 이 문서를 함께 커밋한다. 아래의 “이번 변경은 미커밋”은 11월5일 작업 종료 시점 기록이며, 재개할 때 실제 HEAD/status/origin main을 확인한다.

2026-11-05 최신 결과: [P3 GPU 메모리·reflection 후속](P3-GPU-MEMORY-RESULTS.md), [증거](evidence/p3-gpu-memory/README.md). 직접 헤더/SW 각각 실제 두 프레임115개 검사(46개 GPU buffer/uniform readback)·GL error0·ready→stopped. callback0/부분/전체·중간4-byte 보존·nonzero map/lock·CPU/GPU uniform·grow/recycle·detach/참조 해제 검증 완료. reflection GL 오류의 Wasm trap을 수정하고 shader/GL 실패의 failure→stopped를 확인했다. **P3 진행 중**: 이벤트 Worker 격리 조기 게이트, SW 후속 정책, 전체 종료/재생성, 별도 승인 Pages는 유지한다.

시작 HEAD 및 origin/main은 `04a686e9ac70c6a380ff045ed635723f8861d108`, clean이었다. 이번 변경은 미커밋이며 commit/push·배포 없음. 최종 생산은 `build/p3/gpu-memory-final-002`(Core는 새 `gpu-memory-final-001`의 동일3.1.24 archive·Core 입력 drift0), 계측 없는 host는 `production-memory-final-001`이다. native configure3개·관련14개 실제 GNU C++ 및 Emscripten compile/nm은 통과했지만 전체 native 빌드는 설치된 Core 헤더 부재로3개 실패1, native runtime 미검증이다. 과거 compiler 부재 기록과 구별한다.

이하 날짜가 이전인 미커밋·미push·최초 렌더 미검증 설명은 당시 기록이다. 현재 판정은 위 결과를 우선한다.

2026-10-26 후속 구현: [P3 최초 실제 렌더링 결과](P3-INITIAL-RESULTS.md). 별도 3.1.24 lock/명시 도구 선택, CPU18/5 재검증 완료. Web uniform 이름 길이 조회와 Core CPU uniform persistent mapping 재사용을 수정했다. 실제 Firefox pthread에서 직접 헤더/SW 모두 사각형·배경 두 프레임 픽셀/GL error0 및 ready→stopped를 확인했다. **P3 진행 중**: GPU 부분 갱신·callback·offset·grow/recycle·수명 종합 검사, 이벤트 Worker 격리, SW 후속 정책, Pages·전체 종료/재생성은 남는다. 최종 빌드 build/p3/pixels-3124-002와 docs/evidence/p3-initial을 따른다. 기존 lock/산출물 보존, commit/push/배포 없음.

이하 과거 checkpoint 설명은 위 최신 결과와 함께 읽는다.

2026-10-26 사용자 결정: [원본 Emception 3.1.24 + 동일 SDK 계열 DALi의 서버 없는 컴파일](PLAYGROUND-COMPILATION.md)을 채택했다. 다음 작업 기준은 3.1.24이며 후보 선택 재승인은 필요 없다. 기존 6.0.13 lock은 역사 재현용으로 보존한다. 이번에는 문서와 인수인계만 갱신했으며 다음 구현·검증은 새 컨텍스트에서 진행한다.

2026-10-26 최신 컴파일러 검사: **원본 Emception 3.1.24와 DALi 버전 정렬 검증 통과.** 별도 SDK에서 Core333·생산35 compile/nm·strict 링크0, 브라우저 전체 생산 링크0, 새 사용자 C++ 컴파일/링크0 및 함수 반환5. 직접 헤더와 헤더 없는 SW 모두 공유 메모리·동일 함수 실행 성공. [결과](EMCEPTION-3.1.24-ALIGNMENT.md). DALi host 시작·GPU 렌더·Pages·전체 종료는 미검증이다. 기본 lock6.0.13과 기존 생산 입력497개는 보존했다. 별도 운영 컴파일 서버는 도입하지 않았다.

2026-10-26 컴파일러 후보 추가 검사: 별도 운영 서버 없이 브라우저에서 사용자 C++를 컴파일하는 방향 유지. GameGuild Emception 4.5.0(emsdk 5.0.7)은 공식 구성의 절대 경로 C++/STL 실행에 성공했으나 일반 Emscripten JS 생성 생략과 runner 결함으로 DALi용 후보 게이트 미통과. 이 5.0.7 검사에서는 SDK 전환·DALi 재빌드를 하지 않았다. 이후 원본 3.1.24 정렬 검사는 위 결과를 따른다. [상세 결과](EMCEPTION-5.0.7-FOLLOWUP.md). 아래 P2/P3 상태는 변경하지 않는다.

2026-10-23 (Asia/Seoul). **P2 생산 링크 마일스톤·P2-b 구현 완료, P3 브라우저/GPU 실행 인수 미완료.** P0·R0·R1·R2·P1·P2-a 완료 유지. 현재 기록은 dali-web/docs다.

[현재 결과·특이 사항·미검증 범위](P2-b-PRODUCTION-RESULTS.md), [생산 증거](evidence/p2-b-production/README.md).

1. 실제 Core·생산 GLES Controller·자원·명령·D3 host의 기본 WebGL2 strict 링크를 확보했다. mock Controller·빈 GL/EGL/sync·undefined 억제 없음. final host도 ABORTING_MALLOC=0, FULL_ES3=0이다. 최종 명령/입력/결과는 증거 result.json을 따른다.
2. 생산 Buffer/Controller/Core uniform CPU 통합 18모드, Web wrapper/capability CPU 5모드 통과. allocator shutdown 해제 누락을 재현·수정했다. native Memory2/3와 생성 재시도 정책, 공개 Graphics API는 유지한다. GL 주입 결과는 GPU 보존 근거가 아니다.
3. Web wrapper/cache 파일 분리와 정식 cmake/web source closure, native binary-directory generated cache/pkg-config 출력을 구현했다. GLES/VULKAN/DYNAMIC source 회귀 통과. native configure는 compiler 부재로 3개 실패, 추출/공통 13개 Emscripten compile/nm 통과다. native build/install/runtime은 미검증이다.
4. 실제 host는 main 이벤트 + pthread update/upload/render, 비동기 mailbox ready/failure/stop, fence shutdown→Core 해제→context 폐기→pthread reap/Worker 종료를 연결했다. DOM 경계와 새 Module/Worker/Canvas 재시작을 구현했고 자동 재시도는 없다. 실제 브라우저에서 이 순서를 실행한 증거는 아직 없다.
5. host의 main 8ms/Worker 4ms polling은 초기 correctness 구현이며 성능·숨김/재개·입력·프레임 루프 최적화는 미검증이다. 전체 Core/STL 전역 OOM·runtime abort의 정상 비동기 해제를 보장하지 않는다.

HEAD는 abf62214b23c651f06536138cb8b6e0c0fddbfb7, 기존 Core/Web 변경과 이번 구현 모두 working tree다. commit/push 없음. origin https://github.com/dalihub/dali-web, GitHub 읽기 조회 원격 main a68e3f00f0c7752561137c179500b004b930285e. 원본/P0·lock·과거 증거/산출물 보존. upstream 갱신·운영 서버 구축·Pages 배포 없음. 후속 3.1.24 검사를 위해 별도 SDK 설치와 로컬 브라우저 실행을 수행했다. 원격 SHA는 당시 읽기 조회 기록이며 다음 작업 때 실제 상태를 확인한다.

다음은 별도 3.1.24 프로필/도구 선택 정리와 CPU18/5 재검증 후 P3의 실제 browser 초기화·DALi 픽셀/GL 오류·GPU 보존/수명 검사다. 최초 pthread/browser 로컬 헤더/SW 비교, 별도 승인 실제 Pages, P3/P4 격리 조기 게이트, P4 종료 전 무한 루프 응답성·전체 pthread/Canvas 종료/재생성을 유지한다. Pages+SW 우선, 실패/과도한 비용 시 직접 호스팅 검토, 다른 관리형 호스팅 금지.

[HANDOFF](HANDOFF.md), [PLAN](PLAN.md), [다음 요청문](NEXT-CONTEXT.md). 승인된 Core/Web fail-stop 설계의 재승인은 필요 없다. P2-b-MEMORY-BLOCKER는 승인 전 역사 자료다.
