# P2 생산 링크 이후 인수인계

2026-11-06 캐시 정책 검토: 시작 HEAD/실제 origin main `f81276e13314ae68170b09767782abd229a4a958`, main clean 재확인. [비교·권장안](P3-CACHE-POLICY-REVIEW.md)을 기록했다. 권장은 버전별 불변 자산 묶음·manifest 검증 후 전환·실행별 릴리스 고정·실패 시 기존 완료 묶음 보존이다. **검토 완료, 정책 미합의·미구현·미검증**이며 구현은 사용자 합의 후 진행한다. bootstrap 복구 단위 완료·P3 전체 진행 중, 기존 Firefox9회 및 직접 헤더 E 회귀 결과 유지. 기본 E/독립 C·원본 Emception3.1.24·서버 없는 컴파일·Pages+SW 우선 유지. 실제 Pages 검증·배포는 별도 승인. 이번 변경은 검토/인계 문서뿐이다. 과거 미커밋·복구 승인 대기보다 실제 Git 상태와 이 기록을 우선한다.

## 작업 완료와 컨텍스트 인계 규칙

2026-11-06 사용자 지시: dali-web 작업 단위가 완료돼 다음 컨텍스트로 넘어갈 수 있으면 관련 상태·결과·인계 문서를 갱신하고, 완료한 변경을 commit·origin/main push한 뒤 복사 가능한 다음 프롬프트를 제공한다. 해당 완료 단위의 commit·push를 매번 다시 제안하거나 재승인받지 않는다. 실제 HEAD와 원격 main 일치·작업 트리 상태를 확인해서 보고한다.

미완료 작업이나 명시적 중단/승인 게이트를 완료로 처리하지 않는다. 사용자 변경을 임의로 포함하지 않으며 충돌·push 실패를 force push로 우회하지 않는다. Pages 검증·배포, upstream 갱신·호스팅 전환은 이 commit·push 승인에 포함되지 않는다. 상위 AGENTS의 commit co-author 규칙은 계속 적용한다.

2026-11-06 완료 인계: 사용자 지시로 SW 수명주기 실패 재현·승인 후 bootstrap 복구·검증 증거·관련 문서를 함께 커밋하고 origin/main에 push한다. 이 문서를 포함한 커밋을 다음 기준으로 삼는다. 아래 “미커밋/commit·push 없음”은 검사 종료 당시 기록이다. 실제 push 완료는 최종 채팅의 SHA 및 다음 시작 시 HEAD/status/원격 대조로 확인한다. 작업 완료 시 문서 갱신→commit·push→다음 프롬프트 제공 절차를 HANDOFF에 기록했다. P3 전체 진행 중·Pages 별도 승인 조건은 유지한다.


2026-11-06 SW 복구 승인 후 최신 상태: 사용자 “수정 고고” 승인으로 [bootstrap 복구](P3-SW-RECOVERY-RESULTS.md)를 구현·검증했다([증거](evidence/p3-sw-recovery/README.md)). 격리 성공 시 URL별 retry flag 정리, 자동 reload 최대1회, 등록/제어5초 timeout·늦은 완료 차단, 오류와 다시 시도 버튼을 추가했다. Firefox sw-recovery-003에서 해제→재방문과503/제어없음/지연제어/격리헤더없음 실패→명시적 재시도 통과. 총9회 실제 E 두 프레임·GL error0·ready→stopped, 직접 헤더 E 회귀도 통과. 이전 복구 방향 승인 대기는 해소됐다. **bootstrap 복구 단위 완료·P3 진행 중**, 운영 캐시/전체 offline·다중 탭·브라우저 재시작·Chromium·실제 Pages는 미검증. 테스트의 잘못된 SW는 harness가 제거한 뒤 재시도했으며 자동 SW repair를 구현하지 않았다. 기본 E/독립 C·원본 Emception3.1.24·서버 없는 컴파일·Pages+SW 우선 유지. 공통/native·SDK·lock·SW 원본 변경 없음. 기존748개·CMake 입력376개 drift0. commit/push·배포 없음. 아래 중단·승인 대기는 당시 기록이다.

2026-11-06 SW 후속 최신 상태: 시작 HEAD와 실제 origin/main `99d21f3992c20d60dc0396eda23e43402d2f4842`, clean 확인. [SW 수명주기 결과](P3-SW-LIFECYCLE-RESULTS.md), [증거](evidence/p3-sw-lifecycle/README.md). Firefox 로컬 최초·같은 탭 재방문·SW 업데이트·503 업데이트 실패 후 E 실제 두 프레임/GL error0·ready→stopped 확인. 404·연결 끊김 전달과 CacheStorage 없음도 관측했다. **특이 사항: unregister 뒤 같은 탭 재진입2회가 `SW reload did not isolate`로 실패**한다. 성공 뒤 남은 sessionStorage의 reloaded flag가 재등록을 막는다. P1 D4의 재현 가능한 격리 실패 중단 조건에 따라 복구/캐시 정책 구현 전 중단했다. **P3 진행 중·SW 게이트 미통과, 복구 방향 승인 필요.** 기본 E/C 기존 성공은 유지한다. 정상 소멸·GPU 즉시 회수·장기 누수 없음으로 확대하지 않는다. 운영 캐시/오프라인/자산 버전 혼용·다중 탭·브라우저 재시작·Chromium·실제 Pages는 미검증. 다음 제안은 bootstrap의 성공 후 retry 상태 정리와 명시적 재시도·timeout 및 버전별 캐시 정책 검증이며 미구현이다. 원본 Emception3.1.24·서버 없는 컴파일·Pages+SW 우선 유지. 테스트 harness·증거·문서만 변경, commit/push·배포 없음. 아래 과거 승인 대기/미커밋 기록보다 이 상태를 우선한다.

2026-11-06 커밋 인계: 사용자 요청으로 C 격리 검사 코드·작은 증거·문서를 함께 커밋한다. 아래 “미커밋/commit·push 없음”은 각 검사 종료 시점 기록이다. 재개 시 이 문서를 포함한 실제 HEAD/status와 origin/main을 확인하며 최종 채팅 인계의 push 확인 결과를 우선한다.


2026-11-06 후속 구현: 사용자가 app 이벤트 pthread + window 종료 감독 방식의 구현을 승인했다. [C 조기 검사 결과](P3-EVENT-PTHREAD-RESULTS.md), [증거](evidence/p3-event-pthread/README.md). 기본OFF 독립 C target을 추가하고 기본 E를 유지했다. 새 전체 CMake371단계 빌드0, Firefox 직접 헤더/SW 각각 정상→초기화 무한 루프→timer callback 무한 루프→새 정상 실행4회 행렬 통과. Core/TLS·입력/key signal·timer/trigger 소유권, 실제 픽셀, 무한 루프 중 main heartbeat, 소속2개 pthread counter 정지·Canvas 제거·새 Module/Canvas 렌더를 확인했다. E 픽셀 및 계측 없는 E 정상 종료 회귀도 통과. **C 조기 검사 단위 완료·P3 진행 중.** 강제 종료의 C++/GL 정상 소멸, GPU 메모리 즉시 회수·장기 누수·전체 Adaptor·Chromium 지원까지 선언하지 않는다. 이전 dedicated Worker 중단은 아래 역사 기록이며 현재 승인 대기가 아니다. 다음은 SW 후속 정책/P4 남은 게이트다. 기존748개 보존 drift0, 공통/public·SDK·lock 변경 없음. commit/push·배포 없음.


2026-11-06 최신: [P3/P4 이벤트 Worker 시작 검사](P3-EVENT-WORKER-RESULTS.md), [증거](evidence/p3-event-worker/README.md). 시작 HEAD와 실제 origin/main은 `c98c04796b0b5ebe3c0029a6e37260bd13d6ffb6`, clean이었다. 아래 과거 미커밋 설명보다 이 확인을 우선한다. 기본 E는 동일3.1.24 생산 산출물로 ready→stopped 재통과. dedicated Worker를 event/main-runtime owner로 삼는 C 시작 후보는 pthread/Canvas 전달 실패(exit1), Core 생성 전에 중단했다. **P3 진행 중·C 격리 게이트 미통과.** TLS/이벤트 소유권은 정적 감사만 수행했고 무한 루프 main 응답성·전체 pthread/Canvas 회수·새 인스턴스 재생성은 미검증이다. P1 중단 조건에 따라 D3 확장 방향 결정 전 우회 구현을 하지 않는다. app event pthread + window 종료 감독 방식과 SDK Canvas adapter 대안은 결과 문서의 제안이며 승인/작업큐 등록이 아니다. 이번 변경은 테스트 fixture·문서·작은 증거뿐이며 commit/push·배포 없음. 보존748개·생산499개·runtime3개 hash drift0.


2026-11-06 인계: P3 GPU 메모리 코드·검사·증거와 이 문서를 함께 커밋한다. 아래의 “이번 변경은 미커밋”은 11월5일 작업 종료 시점 기록이며, 재개할 때 실제 HEAD/status/origin main을 확인한다.

2026-11-05 최신 결과: [P3 GPU 메모리·reflection 후속](P3-GPU-MEMORY-RESULTS.md), [증거](evidence/p3-gpu-memory/README.md). 직접 헤더/SW 각각 실제 두 프레임115개 검사(46개 GPU buffer/uniform readback)·GL error0·ready→stopped. callback0/부분/전체·중간4-byte 보존·nonzero map/lock·CPU/GPU uniform·grow/recycle·detach/참조 해제 검증 완료. reflection GL 오류의 Wasm trap을 수정하고 shader/GL 실패의 failure→stopped를 확인했다. **P3 진행 중**: 이벤트 Worker 격리 조기 게이트, SW 후속 정책, 전체 종료/재생성, 별도 승인 Pages는 유지한다.

시작 HEAD 및 origin/main은 `04a686e9ac70c6a380ff045ed635723f8861d108`, clean이었다. 이번 변경은 미커밋이며 commit/push·배포 없음. 최종 생산은 `build/p3/gpu-memory-final-002`(Core는 새 `gpu-memory-final-001`의 동일3.1.24 archive·Core 입력 drift0), 계측 없는 host는 `production-memory-final-001`이다. native configure3개·관련14개 실제 GNU C++ 및 Emscripten compile/nm은 통과했지만 전체 native 빌드는 설치된 Core 헤더 부재로3개 실패1, native runtime 미검증이다. 과거 compiler 부재 기록과 구별한다.

이하 날짜가 이전인 미커밋·미push·최초 렌더 미검증 설명은 당시 기록이다. 현재 판정은 위 결과를 우선한다.

2026-10-26 후속 구현: [P3 최초 실제 렌더링 결과](P3-INITIAL-RESULTS.md). 별도 3.1.24 lock/명시 도구 선택, CPU18/5 재검증 완료. Web uniform 이름 길이 조회와 Core CPU uniform persistent mapping 재사용을 수정했다. 실제 Firefox pthread에서 직접 헤더/SW 모두 사각형·배경 두 프레임 픽셀/GL error0 및 ready→stopped를 확인했다. **P3 진행 중**: GPU 부분 갱신·callback·offset·grow/recycle·수명 종합 검사, 이벤트 Worker 격리, SW 후속 정책, Pages·전체 종료/재생성은 남는다. 최종 빌드 build/p3/pixels-3124-002와 docs/evidence/p3-initial을 따른다. 기존 lock/산출물 보존, commit/push/배포 없음.

이하 과거 checkpoint 설명은 위 최신 결과와 함께 읽는다.

2026-10-26 최신 인계: [원본 Emception 3.1.24 정렬 결과](EMCEPTION-3.1.24-ALIGNMENT.md)를 먼저 읽는다. SDK를 별도 설치하고 실제 Core·생산/host를 전부 재빌드했으며 로컬·브라우저 strict 링크와 새 사용자 C++ 함수 실행까지 성공했다. 기존 hash_memory 문제는 이 조합에서 해소됐다. 로컬 헤더/SW의 모듈 로딩 비교도 통과했지만 dali_web_start는 호출하지 않았다. P3/P4·실제 Pages 게이트는 유지한다. 후속 사용자 승인으로 [원본 Emception 3.1.24 조합](PLAYGROUND-COMPILATION.md)을 채택했다. 다음 작업 기준은 3.1.24다. 기존 lock6.0.13은 과거 재현용으로 보존하며 자동 기본값/별도 프로필 정리는 다음 작업이다. 후보 선택 재승인은 필요 없다. 생산 소스 변경·commit/push 없음.

2026-10-26 추가 인계: [Emception 원본 검사](EMCEPTION-ASSESSMENT.md), [5.0.7 후보 후속 검사](EMCEPTION-5.0.7-FOLLOWUP.md)를 함께 읽는다. 서버 없는 사용자 소스 컴파일이 목표다. 5.0.7 후보는 C++/STL 실행만 확인됐고 JS 생성 경로 결함으로 채택하지 않았다. 단순 버전 정렬 문제가 아니며, 공식 preset의 실행 성공을 DALi strict 링크로 취급하지 않는다. 기존 6.0.13·생산 산출물 유지. 도구 JS 실행 계층 포팅이나 DALi 전용 runtime 설계는 아직 선택하지 않았다.

2026-10-23. P2-b 구현과 P2 strict 링크 게이트를 완료했다. **실제 browser/GPU·비동기 종료 실행 인수는 P3/P4에 남는다.** 최신 사실은 [STATE](STATE.md), [결과](P2-b-PRODUCTION-RESULTS.md), [생산 증거](evidence/p2-b-production/README.md)를 따른다.

## 먼저 읽을 것

상위 AGENTS.md, README, STATE·PLAN·NEXT-CONTEXT, PLAYGROUND-COMPILATION과 EMCEPTION-3.1.24-ALIGNMENT 및 evidence/emception-3.1.24-alignment/README, P2-b-PRODUCTION-RESULTS와 evidence/p2-b-production/README, P2-b-WEB-MEMORY-RESULTS와 evidence/p2-b-web-memory/README, P2-b-CORE-FAILURE-RESULTS, P1-EXECUTION-CONTRACT, P2-a-MANIFEST·P2-a-RESULTS, P2-b-HANDOFF·P2-b-SYNC-RESULTS, BUILDING·UPSTREAMS·MIGRATION 및 upstreams/toolchain lock. MEMORY-BLOCKER는 승인 전 역사 기록이다.

## 현재 코드와 검증

1. HEAD abf62214b23c651f06536138cb8b6e0c0fddbfb7, main, dirty. 기존 Core internal 변경은 그대로 보존하고 Web 메모리·wrapper/cache·host·CMake·검사 코드를 더했다. commit/push 없음. 원격 GET main a68e3f00f0c7752561137c179500b004b930285e. 실제 status부터 확인한다.
2. 생산 Buffer/Controller CPU 통합 controller-007: 33개 compile/nm·링크·Node18모드0. 실제 grow/recycle·allocator와 객체 allocation 실패·create/discard·CPU/GPU Core uniform·Memory 보유 shutdown·후속 command 차단. 첫 004는 shutdown allocator free 누락을 재현했고 수정했다. Web GL gl-unit-002의5모드도0. 모두 주입 CPU 검사이며 GPU 증거가 아니다.
3. WebGlImplementation은 실제 GL 호출과 명시적 미지원 실패를 제공한다. native map/advanced blend/binary/native extension import를 선택하지 않는다. Program binary cache는 native/Web 파일 선택이며 Web에서는 source shader만 사용한다. 공개 Graphics API·native Memory2/3·native retry 유지.
4. cmake/web은 actual Core + production GLES + web/host를 링크한다. 최종 production-003에서 Core 333단계·35개 compile/nm·strict0을 확인했다. 기본 WebGL2/FULL_ES3=0, ERROR_ON_UNDEFINED_SYMBOLS=1, ABORTING_MALLOC=0, pthread/OffscreenCanvas, main blocking 금지다. source manifest/nm/full map과 작은 발췌를 보존했다.
5. D3 host의 ready/frame/failure/stop은 main/Worker release/acquire mailbox다. main은 Core 이벤트·DOM·timer, pthread는 Update·upload·GL·fence다. 최대 한 frame만 진행하며 main8ms/Worker4ms polling 비용·브라우저 실행은 미측정이다. 순서는 event 정리→owner ContextDestroyed/Controller/fence shutdown→main Core/TLS 해제→owner context 폐기/pthread 종료→main tryjoin/Worker 종료다. 같은 모듈 재시작 금지, JS는 명시적인 새 Module/Canvas를 만든다.
6. source 회귀0, closure-002의 Web 집합·native generated 분리0. production-native-002 native configure3개1(compiler 없음), native 추출/공통13개 Emscripten compile/nm0. native GL·shader 파일 cache·custom allocator shutdown·Core render 실행은 미검증이다. 전체 Tizen Adaptor WEB profile은 계속 거부하며 독립 최소 target만 제공한다.

## 재개 경로와 다음 작업

이번 사용자 요청은 문서 갱신과 새 컨텍스트 준비까지다. 아래 작업은 아직 실행하지 않았다.

- 로컬 환경: `logs/emception/alignment-3124-001/env.sh`. 수동 추출 SDK `tools/emsdk-3.1.24`에는 emsdk_env.sh가 없다. [BUILDING](BUILDING.md)의 명시 도구/config/cache 선택을 사용하고 버전을 확인한다. 개인 경로를 필수화하지 않는다.
- 성공한 빌드: `build/emception/alignment-3124-002`; 로그 `logs/emception/alignment-3124-native-002`, `logs/emception/alignment-3124-browser-001`, `logs/emception/alignment-3124-runtime-headers`, `logs/emception/alignment-3124-runtime-sw`.
- 보존한 브라우저 harness: `logs/emception/alignment-3124-001/probe-aligned.py`, `probe-runtime.py`; 정규화한 재현 자료는 위 evidence 디렉터리. 대용량 파일은 commit하지 않는다.
- 먼저 기존 lock을 보존하는 별도 3.1.24 프로필/lock과 도구 선택을 정리하고 Controller CPU18/wrapper CPU5를 이 SDK에서 재실행한다. 필요하면 새 경로에서 생산 strict 링크/source closure를 재확인한다. 6.0.13 산출물을 혼용하지 않는다. 이전 실패한 native-001은 SDK config/cache 변경 중 발생한 검사 환경 오류로 보존한다.

## P3 실행과 유지할 게이트

1. P3 실제 browser 초기화·사각형/배경 픽셀·GL 오류를 실행한다. 첫 pthread/browser 단위에 로컬 COOP/COEP 헤더와 헤더 없는 SW 격리 비교를 함께 준비한다. 로컬 loopback 임시 정적 파일 제공으로 검사하며 운영 서버 구축·배포는 하지 않는다. 기존 모듈 로딩 비교 성공을 Worker GL·DALi 렌더링 성공으로 대신하지 않는다. 실제 Pages는 별도 승인이다.
2. 16-byte/중간4-byte·callback0/부분/전체·nonzero map/lock offset·CPU/GPU uniform·두 frame·grow/recycle·수명을 실제 GPU readback/픽셀로 확인한다. CPU 배열·FULL_ES3 링크를 보존 근거로 쓰지 않는다. void Unlock/Unmap 이후 Core bool을 GL 성공/GPU 완료로 해석하지 않는다.
3. host 비동기 ready/failure/stop, 정상/실패 종료와 새 Module/Worker/Canvas 재생성은 아직 runtime 미검증이다. 코드가 연결됐다는 사실을 종료 인수 통과로 확대하지 않는다. 전체 Core/STL OOM·runtime abort는 정상 종료 보장 밖이다.
4. P3/P4 이벤트 Worker 플레이그라운드 격리 조기 검사, P4 종료 전 무한 루프 응답성·전체 실행 정리/재생성을 유지한다. 기본 E의 main 이벤트 모델이 임의 코드 격리를 보장하지 않는다.
5. Pages+SW 우선, 실패/과도한 비용이면 직접 호스팅 검토. 다른 관리형 호스팅 금지. 원본/P0/SDK/lock/과거 산출물 보존, 새 build/log, 최대-j6. upstream 갱신·배포·서버 구축 금지 조건은 별도 승인 전 유지한다.

승인된 Core 실패 처리·Web fail-stop 설계를 다시 승인받지 않는다. 승인 범위를 넘는 공통/public 계약 변경·전체 플랫폼 포팅·실험 확대·합의 변경이 필요할 때만 중단·보고한다.
