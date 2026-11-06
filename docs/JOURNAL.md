# DALi 브라우저 포팅 작업 이력

[001~008 전체 이력](archive/probe/docs/JOURNAL.md): P0, R0, R1-a/R1-b, Tizen 기준 전환, R2-a 이관·Core 검증. 원문 이력은 보관하고 이후 항목은 여기에 추가합니다.

## 009 R2-b 브라우저·Adaptor·GL 재검증과 기록 전환

2026-10-23 (Asia/Seoul). R2-b 및 상위 R2 완료, P1 미착수.

- R2-a HEAD `728cf818d4db74890a4c37d01737c8eea3d9a14b`와 clean 상태, 원격 일치 확인. lock과 upstream은 그대로 유지.
- 현재 Core 산출물의 Firefox mock 애니메이션 50/100 종료 0. 현재 Adaptor 6개 번역 단위 각각 종료 0. GL 탐색 Python 0, 기본/FULL_ES3 링크 각각 1, P0와 동일한 265개·미해결 7개/3개.
- 컴파일·링크 개별 명령/종료 코드 기록 보강. 생산 코드·API 변경 없음. [결과와 검증 한계](R2-b-RESULTS.md), 작은 증거 `docs/evidence/r2-b/`, 전체 로컬 로그 `logs/r2-b/run-001/`.
- 특이 사항: 최초 sandbox 소켓·snap·DNS 제한으로 실패, 실패 로그 보존 후 호스트 권한 재시도 통과. GL 링크 실패는 예상 탐색 결과이며 생산 링크 성공으로 판정하지 않음.
- 원본/P0 HEAD·status·diff, 사용자 파일·기존 로그·lock·Core 산출물 hash 불변 확인. SDK 재사용, Core 재빌드/Node 재실행 없음.
- [이관 목록](MIGRATION.md)에 파일 매핑·미이관 자료·문서 링크 정책 기록. 기존 문서를 archive에 보존한 뒤 새 docs를 기록 기준으로 전환. 기존 README·STATE·HANDOFF 등에 새 경로 안내.
- 계획·현재 상태·P1 인수인계·다음 프롬프트 갱신. 생산 렌더러·스레드 설계·Pages 배포 수행 없음. P1 실행 전 중단.
- 생성물은 ignored build/logs 아래. 남긴 브라우저·서버·빌드 프로세스 없음. SDK·프로필·대용량 로그 commit 제외.

후속 항목에는 변경 파일·revision, 명령/종료 코드·증거 위치, 실패 원인·특이 사항, 미검증 범위·다음 행동·잔여 프로세스를 함께 기록합니다.

## 010 P1 실행 계약 초안과 결정 게이트

2026-10-23 (Asia/Seoul). **P1 미완료·결정 대기, P2 미착수.**

- 시작 HEAD `ef6a1f545ff03df69c3297135048f545ee8dd352`, clean main. 최초 sandbox remote 조회는 DNS 오류 exit 128, 호스트 권한 조회 exit 0으로 원격 main 일치 확인. upstream/lock 유지.
- [P1 실행 계약](P1-EXECUTION-CONTRACT.md): 현재 Core·GLES 호출 순서, 실제/논리 context, upload·frame·cleanup, 미지원 GL/EGL 정책, thread/hosting 후보, P2 source closure·변경 파일·검증 게이트 기록.
- 특이 사항: 현재 Memory3 WRITE-only를 SDK가 거부함. Memory2의 uninitialized staging 전체 upload는 partial callback 보존 대안이 아님. Controller/sync pool의 concrete EGL/native fence 결합을 확인하고 변경 경계를 설계에 포함. 생산 수정 없음.
- SDK 6.0.13 access guard를 추출해 Node 24.19.0에서 실행, exit 0. WRITE-only 반환 0, invalidate 포함은 guard만 통과. 브라우저·GL·데이터 보존·pthread runtime 검증 아님. [검사·명령·한계](evidence/p1/README.md), 로컬 `logs/p1/run-001/`.
- 온라인 공식 문서와 SDK 소스로 pthread/SAB/격리 조건 및 EGL 제한 검토. Pages 실제 배포·응답 헤더/runtime 검증 없음. 기존 R2-b 결과 재실행 없음.
- D1 기능/context, D2 upload 의미, D3 thread owner, D4 격리 hosting 미결. P1 완료 조건을 막으므로 완료 선언하지 않음. README·PLAN·STATE·HANDOFF·NEXT-CONTEXT 갱신. 다음은 사용자 설계 결정이며 P2 자동 실행 아님.
- 원본/P0 Git·사용자 파일·과거 로그·R2-b 증거·lock·Core 산출물 전후 보존 검사와 문서 검사 결과를 P1 evidence에 기록. SDK·생산 소스·build 변경 없음. 남긴 서버·브라우저·빌드 프로세스 없음.

## 011 P1 D1 아키텍처 결정 반영

2026-10-23 (Asia/Seoul). 사용자 승인으로 D1 설계 확정, D2~D4 미결. P1 미완료·P2 미착수.

- 공통 GLES 렌더러 + Native/EGL·Web 환경별 파일의 빌드 시 선택을 채택. 기존 HTML5 어댑터 우선 잠정안을 책임 분리 중심의 정식 포팅 설계로 구체화.
- Core 계약과 기존 GLES/Vulkan/DYNAMIC 유지, concrete EGL cast·타입·native FD를 native 구현으로 이동하는 경계, 컨텍스트·업로드·sync 분리 및 실행 시 capability 확인 명시.
- P2 source 선택·변경 파일·native GLES/Vulkan/DYNAMIC 회귀·웹 native 의존 배제 검증 보강. 실제 파일명·세부 내부 인터페이스는 구현 계획에서 확정하며 생산 코드 변경 없음.
- PLAN·STATE·HANDOFF·NEXT-CONTEXT·README 동기화. 다음 논의는 D2의 버퍼 부분 쓰기 보존과 CPU shadow 비용. D3/D4 미결 보존, D1 승인으로 다른 항목이나 구현을 승인한 것으로 간주하지 않음.
- 시작 clean main `d2964ae0910cc475992ed1c396247db150791b68`, 원격 일치 확인. 검증은 문서 링크·공백·변경 범위 검사이며 빌드/runtime 검증은 수행하지 않음. 기록: `logs/p1/d1-design-001/`. 기존 SDK·소스·lock·P0/R2/P1 실행 증거 보존, Pages 배포 없음.

## 012 P1 D2 웹 버퍼 업로드 결정 반영

2026-10-23 (Asia/Seoul). 사용자 승인으로 D2 설계 확정. D1·D2 확정, D3·D4 미결, P1 미완료·P2 미착수.

- 별도 웹 메모리 구현과 보존이 필요한 버퍼 소유 CPU shadow 채택. native Memory2/3 유지, Memory4 추가 없음. 임시 매핑 객체와 지속 저장소의 소유권 분리.
- 명시적 구간 업로드와 callback 매핑을 구분하고 shadow 일관성, valid range·discard/recycle·확장·실패·GPU write 경계 및 메모리 비용 검증 명시. 전체 덮어쓰기만 허용하는 기능 축소안은 미채택.
- P2 변경 범위와 PLAN·STATE·HANDOFF·NEXT-CONTEXT·README 갱신. 다음 논의는 D3 실행 주체. D3·D4 권장안은 미승인이고 생산 코드 구현은 시작하지 않음.
- 시작 clean main `b5362821a3834c44b0a859bc9829a4e49717c349`, 원격 SHA 일치. 문서 링크·공백·변경 범위 검사만 수행, runtime/build 미실행. 실행 기록 `logs/p1/d2-design-001/`. 소스·lock·SDK·기존 증거·원본 사용자 변경은 수정하지 않음.

## 013 P1 D3 기본 실행 구조와 격리 조기 게이트

2026-10-23 (Asia/Seoul). 사용자 승인으로 D3 확정, D1~D3 확정·D4 미결. P1 미완료·P2 미착수.

- 생산 기본 E: 브라우저 main = DALi 이벤트, 별도 pthread Worker = 업데이트·업로드·렌더. 이전 C 우선 권장안을 대체. DOM 연결과 이벤트 로직 분리, main blocking을 비동기 ready/stop으로 전환하는 계약 반영.
- Core TLS·Adaptor 이벤트 플래그와 플랫폼 main-loop 직접 결합을 구분한 소스 조사 기록. 전체 Worker 호환성 감사나 실행 성공으로 확대 해석하지 않음.
- 이벤트 Worker는 플레이그라운드 격리 후보로 유지. P2 이후 P3/P4 구간에서 TLS·callback·timer·종료·무한 루프·전체 인스턴스 종료/재생성을 검증하고 P4 종료 전 결과·장애 기록. 두 실행 구성 동시 지원을 확정하지 않음.
- PLAN·STATE·HANDOFF·NEXT-CONTEXT·README와 P2 변경 범위 갱신. 마지막 논의는 D4 격리 호스팅. 소스·lock·SDK·원본·기존 증거 변경 및 Pages 배포 없음.
- 시작 clean main `f686f106f8d99d218ab54dfbb029ef0fe1ddb722`, 원격 동일. 문서 검사만 수행, build/runtime 미검증. 기록 `logs/p1/d3-design-001/`.

## 014 D4 호스팅 정책 확정과 P1 설계 완료

2026-10-23 (Asia/Seoul). D1~D4 사용자 결정 확정, P1 설계 완료·P2 미착수.

- D4: GitHub Pages + SW 격리 우선 검증, 실패/과도한 비용 시 직접 호스팅 전환 검토. 사용자 제약으로 타사 관리형 호스팅 제외. D3 유지, 배포·서버 구축은 별도 승인.
- URL 기준 경로와 SW bootstrap 분리, 최초 pthread/browser 검증 단위에 작은 호스팅 probe 준비. 실제 Pages 검증 전 지원 완료 판정 금지, 로컬 성공과 실제 배포 성공 구분.
- 설계 문서 7절에서 P1 완료 조건별 근거와 후속 미검증 게이트 점검. 미결 설계 결정 없음. 실제 생산 링크·픽셀·스레드·호스팅은 검증 완료 아님.
- 다음 컨텍스트는 P2-a source manifest·컴파일/strict 링크 의존성 진단. 대규모 구현은 P2-b로 넘기고 진단 후 인수인계. P2-a 진단 완료를 상위 P2 생산 링크 완료로 간주하지 않음.
- README·PLAN·STATE·HANDOFF·NEXT-CONTEXT 갱신. 시작 clean main `e471178f2cb9bc96f0f389fb52cb67c4c2d228ae`, 원격 동일. 문서 링크·공백·변경 범위 검사만 수행. 기록 `logs/p1/d4-close-001/`. 소스·lock·SDK·기존 증거·원본 사용자 변경 불변, build/runtime/배포 미실행.

## 015 P2-a 생산 GLES manifest·strict 링크 진단

2026-10-23 (Asia/Seoul). **P2-a 진단 완료·P2-b 미착수·상위 P2 미완료.**

- 시작 clean main 및 원격 main `3eb2c89ecad10f810d2ff064f2b4f3d43a42a426`. sandbox 원격 조회 DNS 실패 exit 128, 호스트 조회 exit 0으로 일치 확인. upstream/lock 변경 없음.
- 명시적 생산 GLES 23개 + wrapper/utility/generated/harness 포함 30개 manifest와 독립 CMake·진단 script 추가. 실제 Controller/Core 생성·초기화·frame·native sync를 KEEPALIVE로 참조, mock/helper·빈 함수·undefined 억제 없음. 생산 libs 변경 없음.
- 최초 configure(001) exit 1: shader-cache-path는 native 생성물. binary directory에 실제 템플릿 생성 후 002 configure/Core 333단계 및 30개 compile/nm 통과. 초기 링크 10/6개 미해결, 각각 exit 1.
- 특이 사항: 초기 harness에서 GC된 native 경로를 추가 참조한 003 재진단. 새 Core 333단계·30개 compile/nm 각각 exit 0, 기본/FULL_ES3 strict 링크 각각 exit 1, 미해결 16/12개. map 미생성. Python exit 0은 결과 수집 성공만 의미.
- EglGraphics RTTI/context 3개, NativeFence/EglSyncObject 3개, shader binary utility 5개, GL 5/1개 분류. FULL_ES3 매핑 4개 해소는 runtime/보존 근거가 아님. [결과](P2-a-RESULTS.md), [manifest](P2-a-MANIFEST.md), [P2-b 파일·적용 순서·회귀](P2-b-HANDOFF.md).
- 원본/P0 Git·사용자 파일·P0/R2/P1 로그·증거·lock·기존 Core 산출물 전후 검사, 생산 diff 없음. SDK 재사용·빌드 최대 -j6. 산출물/전체 로그 ignored, 공개 증거는 작은 정규화 사본만 포함.
- runtime·브라우저·실제 픽셀·Worker/OffscreenCanvas·Pages·native 회귀 미검증. 배포·서버 구축 없음. 실행한 빌드 종료, 남긴 서버/브라우저 없음.
- P2-b 진행 근거 확보. 합의한 범위 내 진단 종료 후 STATE/PLAN/HANDOFF/다음 프롬프트 갱신, 이번에는 P2-b 구현 전 멈춤.

## 016 P2-b source/context 중간 구현과 검증

2026-10-23 (Asia/Seoul). **P2-b 진행 중·상위 P2 미완료.** 요청된 전체 P2-b 중 source/context 단위의 중간 인수인계다.

- 시작 clean main HEAD/원격 `a68e3f00f0c7752561137c179500b004b930285e`. sandbox 원격 조회 DNS 실패 후 읽기 전용 호스트 조회로 일치 확인. upstream/toolchain lock 불변.
- GlesGraphicsController rename 및 모든 production/test/probe 참조 갱신. alias/중복 Controller 없음. 공통 초기화는 기존 GraphicsSyncAbstraction 보관, context/RTTI는 native 파일로 이동. Core 공통 public 계약 변경 없음.
- native 기본 source 선택과 WEB 조합 검증 추가. 미완성 production WEB closure는 configure 거부, cmake/p2-b만 context 경계 진단. Web은 단일 cache/identity 경계만 구현하고 실제 host는 아직 없음.
- 특이 사항: 공통 Context에도 EGL context API 8개 존재. NativeRendering 내부 파일 경계로 생성·복원·삭제 이동, Web의 해당 EGL 요청 명시 거부. 별도 native rendering context 수명 회귀는 미검증.
- 최종 context-web-003: 새 pthread Core 333단계 및 32개 compile/nm 각각 exit 0. strict 기본/FULL_ES3 각각 exit 1, 미해결 13/9개, map 없음. 이전 001 전이 include 실패와 002 결과도 보존. mock/빈 GL·EGL·sync/undefined 억제 없음.
- GLES/VULKAN/DYNAMIC source 보존 및 잘못된 조합 거부 검사 통과. native configure는 C/C++ compiler 부재로 각각 exit 1. native context/rendering/egl-graphics는 Emscripten 교차 compile/nm 각각 exit 0이며 host-native build/runtime 성공으로 해석하지 않음.
- 원본/P0 Git 상태 및 과거 보존 스냅샷 일치, 추가 보존 대상 1,370개 hash 불변. 전체 입력 hash와 최종 compile 입력 일치. SDK·산출물·큰 로그 commit 제외, 최대 -j6. 원격 push·upstream 갱신·서버·브라우저·Pages 배포 없음.
- [중간 결과](P2-b-RESULTS.md)·[증거](evidence/p2-b/README.md), STATE/PLAN/HANDOFF/NEXT-CONTEXT 갱신. 다음은 sync 분리 → 웹 메모리 → wrapper/capability → cache → 실제 host/strict 링크. native runtime·P3 픽셀·D3/D4 초기 probe·P3/P4 격리 게이트 유지.

## 017 P2-b sync 환경 분리와 중간 수명 검증

2026-10-23 (Asia/Seoul). **P2-b sync 중간 구현·상위 P2 미완료.** 다음은 웹 메모리이며 전체 strict 생산 링크 요청의 나머지 범위는 유지한다.

- 시작 clean main `7b6ee23ff36b3658fc957e1607de2d21963796c1`. sandbox DNS 실패 후 읽기 전용 origin 조회 exit 0, 원격 main `a68e3f00f0c7752561137c179500b004b930285e`. upstream/toolchain lock 변경·원격 push 없음.
- SyncPool concrete EGL storage/FD/cast를 native 파일로 추출하고 native 정책 보존. texture dependency policy·Controller sync factory도 환경별 선택. allocation/recycle helper 추출, Core·GraphicsInterface·GraphicsSyncAbstraction 계약 변경 없음.
- WebFence의 실제 fence/zero-timeout poll, pending/complete/failed/cancelled, owner 삭제/다른 스레드 해제 요청, 종료 cancellation 구현. frame age/flush 완료 판정 없음. 같은 frame shared fence 재사용 및 native interop/cross-context wait를 Web에서 허용하지 않음.
- 특이 사항: Core bool 완료 계약과 별도로 내부 failure latch 제공. 실제 host의 비동기 오류/stop 연결은 남았다. 완료 상태가 아닌 실패를 완료로 위장하지 않는다. 실제 GPU/브라우저 sync 의미는 아직 미검증.
- sync-web-001/002/003 각각 새 Core 333단계·34개 compile/nm 통과, strict 기본/FULL_ES3 각각 exit 1·10/6개 미해결·map 없음. 최종은 003(최종 헤더 표기 포함). NativeFence 미해결 3개 제거, cache utility/GL mapping/BlendBarrier는 남음.
- fence-unit-001/002 compile/link·Node 각각 exit 0. 실제 WebFence 코드 + 주입 GL 응답의 수명 검사이며 생산 링크/GPU 증거가 아님. 테스트 전용 --wrap은 생산 진단에 사용하지 않음.
- sync-sources-001 native GLES/VULKAN/DYNAMIC 목록 회귀 통과. sync-native-001/002 세 native configure는 compiler 부재로 실패, 최종 추출 5개 Emscripten 교차 compile/nm 통과. native build/install/runtime과 구분.
- 이전 snapshot 1,370개, P2-a snapshot 176개, 이번 시작 증거/lock 36개 hash 일치(중복 포함). 원본/P0 Git 상태·사용자 변경 보존. 개인 SDK 필수화·SDK 설치·upstream 갱신·서버·브라우저·Pages 배포 없음. 새 build/log·최대 -j6.
- [sync 결과](P2-b-SYNC-RESULTS.md)와 [별도 증거](evidence/p2-b-sync/README.md), PLAN/STATE/HANDOFF/NEXT-CONTEXT 갱신. 현재 세션 turn_context의 모델 `gpt-6-astra`를 확인해 로컬 checkpoint co-author에 사용한다.
- 자원 생성/삭제·스레드 수명까지 포함한 sync 단위에서 중간 인수인계. 웹 메모리 → wrapper/capability → cache → 실제 D3 host/strict 생산 링크 및 source/generated 정리는 미완료다. P3 픽셀·최초 pthread/browser 헤더/SW/Pages·P3/P4 격리 게이트 유지. 합의 방향 안에서 다음 웹 메모리 구현 진행 가능.

## 018 P2-b 웹 메모리 착수 조사와 실패 처리 게이트 중단

2026-10-23 (Asia/Seoul). **Core 내부 실패 처리 결정 대기, 전체 P2-b·상위 P2 미완료.**

- 시작 clean main `abf62214b23c651f06536138cb8b6e0c0fddbfb7`. GitHub 연결의 읽기 전용 ref 조회로 원격 main `a68e3f00f0c7752561137c179500b004b930285e` 확인. commit/push·upstream/lock 변경 없음.
- 필수 문서와 실제 map/lock·vertex/index/uniform 호출부 조사. P1의 null 문제를 재확인하고 상위 void 완료 처리·dirty 해제까지 추적. map/lock null 반환 자체는 기존 공개 Graphics 계약이며 공개 API 변경이 반드시 필요하다는 판정은 아님.
- 특이 사항: GpuBuffer의 무검사 역참조/memcpy/callback, vertex/index의 업로드 성공 가정, uniform의 실패 미전달 때문에 단순 Web 메모리 null 반환만으로 호출부 안전성을 충족하지 못함. 사용자의 공통 실패 처리 변경 전 중단 조건을 적용. Core 내부 실패 전파·draw 처리 또는 Web fail-fast를 임의로 구현하지 않음.
- [중단 보고·미승인 선택지](P2-b-MEMORY-BLOCKER.md), [정적 증거](evidence/p2-b-memory-contract/README.md), PLAN/STATE/HANDOFF/NEXT-CONTEXT 갱신. 원래 웹 메모리 → wrapper → cache → host → strict 생산 링크/source/generated 정리 범위는 유지.
- 생산 코드·빌드 설정 변경 없음. 이번 검증은 소스 추적·문서·변경 범위·증거 보존 검사뿐이며 compile/nm/link·CPU/GPU 메모리 검사·native 회귀는 실행하지 않음. 기존 sync 결과 34개 compile/nm 성공·strict 10/6개 미해결은 과거 증거로 유지.
- SDK·원본/P0·과거 증거/산출물 보존. 새 로그 `logs/p2-b/memory-contract-001/`, SDK/빌드/브라우저/서버/배포 실행 없음. P3 GPU 보존·D3/D4 호스팅·플레이그라운드 격리 게이트 유지. 다음 행동은 실패 처리 정책과 수정 범위 결정.

## 019 P2-b Core 실패 처리 승인·구현과 중간 검증

2026-10-23 (Asia/Seoul). 사용자 “오케이 진행”으로 공개 Graphics API 유지·Core 내부 실패 전파·Web fail-stop 연결 설계를 승인받았다. 두 abandon Core 패치에 의존하지 않고 구현했다. 전체 P2-b·상위 P2는 미완료.

- GpuBuffer create/map/lock 실패 검사와 bool 결과, vertex/index dirty 유지·callback lock 해제, Geometry bind/draw 유효성 검사. 정상 callback 0-byte와 실패 구분.
- CPU/GPU uniform·view 범위/overflow·준비 실패 latch, scene uniform 총 크기 확장. 특이 사항: 공유 UBO 실패의 영향이 기록된 다른 draw에도 미치므로 최종 scene/pass 제출 gate를 추가. sync/tracker/readback 소비를 gate 뒤로 이동.
- `core-failure-005`: pthread Core 포함 375단계 build·실패 주입 Node·기존 애니메이션 50/100 모두 exit 0. `001/002` compile 실패와 `004` 테스트 REQUIRES_SYNC 설정 누락 실패는 보존. CPU mock 검사이며 실제 GPU 보존 근거 아님.
- `core-failure-sources-001` GLES/VULKAN/DYNAMIC source 회귀 0. `core-failure-native-001` native configure 3개는 compiler 부재로 각각 1, 기존 native 추출 5개 Emscripten compile/nm은 각각 0. native Core/Adaptor build/install/runtime 미검증.
- [결과·재현·특이 사항](P2-b-CORE-FAILURE-RESULTS.md), [작은 증거](evidence/p2-b-core-failure/README.md). Core 변경은 internal render 경로만, 공개 Graphics API·Adaptor·Memory2/3·원본·SDK·lock은 보존. HEAD `abf62214b23c651f06536138cb8b6e0c0fddbfb7`, working tree 변경, commit/push 없음.
- PLAN·STATE·HANDOFF·NEXT-CONTEXT의 승인 대기 상태 해제. WebBufferMemory/shadow·Web failure latch·wrapper/cache·실제 D3 host·생산 strict 링크·source/generated 정리는 남았으며 기존 승인 범위 안에서 계속 가능. P3 GPU/P4 격리·Pages 별도 승인 게이트 유지.
- `core-failure-web-001` 재진단: pthread Core333·34개 compile/nm 각각0, strict 기본/FULL_ES3 각각1·미해결10/6·map 없음. 실제 host 없이 부분 생산 객체 진단이며 전체 P2 미완료 유지.

## 020 P2-b Web 메모리 중간 구현과 CPU 계약 검사

2026-10-23 (Asia/Seoul). 기존 Core dirty 변경을 보존하고 승인된 단계 4를 진행했다. 전체 P2-b·P2 미완료이며 생산 Controller 통합 검사와 host 연결이 남았다.

- HEAD abf62214b23c651f06536138cb8b6e0c0fddbfb7 유지, 읽기 조회 원격 main a68e3f00f0c7752561137c179500b004b930285e. commit/push 없음.
- MapBufferRange·Buffer native/Web 파일 분리, native Memory2/3 그대로. WebBufferMemory/Storage의 lazy shadow·offset/bounds·참조 수명·detach, WebFailure allocation/GL/context/fence latch 및 Controller 제출 gate. Web 재시도 없음, native 재시도 유지, create/discard 참조 정리.
- 특이 사항: 매핑 API에 full overwrite/callback 힌트가 없어 매핑된 GPU 버퍼는 shadow를 사용한다. 제어 블록 할당 없이 저장소를 pin하고 recycle revision 충돌을 방지했다. Core bool은 GL/GPU 성공이 아니다. host 비동기 failure/stop 연결은 아직 미구현이다.
- memory-unit-003 compile/link·Node12모드 각각0. 실제 GPU/생산 Controller가 아닌 저장소·매핑 CPU/GL 주입 검사. 생산 grow/recycle·CPU/GPU uniform·queue/shutdown 통합 검사는 후속이다.
- memory-web-003 Core333·37개 compile/nm 각각0, strict 기본/FULL_ES3 각각1·10/6개 미해결·map 없음. 001 bit-field 전달 compile 실패, 002 중간 성공도 보존.
- memory-sources-001 source 회귀0. memory-native-002 native configure3개1(host compiler 부재), 추출7+공통 Controller/Buffer·Memory2/3=11개 Emscripten compile/nm0. native build/runtime 미검증.
- [상세 결과](P2-b-WEB-MEMORY-RESULTS.md), [증거](evidence/p2-b-web-memory/README.md), PLAN/STATE/HANDOFF/NEXT-CONTEXT 갱신. 기존 Core·증거·lock 89개 hash 일치, 최종 boundary 입력 drift 없음. 원본/P0·SDK·과거 산출물 변경 작업 없음. 배포·서버 구축·upstream 갱신 없음.
- 검증 가능한 저장소·매핑 단위에서 중간 인수인계한다. 다음은 생산 Buffer/Controller 통합 검사 보강, 이어 wrapper/capability→cache→D3 host→기본 strict 링크/source/generated 정리다. 기존 승인과 P3/P4·D3/D4 게이트를 유지한다.

## 021 생산 Buffer/Controller 통합 checkpoint

2026-10-23 진행 checkpoint: [생산 Buffer/Controller CPU 통합](evidence/p2-b-controller-memory/README.md) controller-005의 compile/nm·테스트 링크·12모드 통과. shutdown custom allocator 해제 누락을 재현·수정했다. Web wrapper/capability 구현을 진행 중이며 host/cache/생산 strict 링크·최종 회귀는 아직 미완료다. 승인 범위와 전체 실행을 계속 유지한다.


## 022 — 2026-10-23 P2-b 생산 구현과 P2 strict 링크

실제 Buffer/Controller/Core uniform 통합 검사, Web wrapper/capability와 shader binary cache 경계, 실제 D3 host 및 정식 cmake/web closure를 완료했다. failure latch→제출 차단→main 비동기 failure/stop과 새 Module/Worker/Canvas 실행 정책을 연결했다. generated cache/pkg-config 출력은 binary directory로 이동했다.

특이 사항: custom allocator 객체의 shutdown 해제 결함을 실제 Controller 검사에서 재현·수정했다. host polling은 초기 correctness 구현이다. 전체 Core/STL OOM과 runtime abort의 정상 종료를 보장하지 않는다.

production-003 Core 333단계·생산/host35 compile/nm·기본 strict 링크 각각0. controller-007 18모드, gl-unit-002 5모드 통과. GLES/VULKAN/DYNAMIC source 및 generated closure 회귀 통과. native configure3개는 host compiler 부재로 각각1, 추출/공통13개 교차 compile/nm 각각0. native build/install/runtime과 browser/GPU/픽셀/실제 shutdown은 미검증이다.

[증거](evidence/p2-b-production/README.md), [결과·미검증 영향](P2-b-PRODUCTION-RESULTS.md). P2 링크 마일스톤 완료, 다음 P3 실제 GPU 및 최초 pthread/browser 헤더/SW 비교. 실제 Pages는 별도 승인, P3/P4 격리 조기 게이트와 P4 전체 종료/재생성 유지. 기존 Core·과거 evidence·lock 보존. commit/push·upstream 갱신·서버/배포 없음.


## 023 — 2026-10-26 서버 없는 사용자 C++ 컴파일 후보 후속 검사

사용자가 예비 결과를 최종 실패로 이해했다고 정정하며 남은 확인을 요청했다. GameGuild Emception 4.5.0/emsdk5.0.7을 공식 Next/Turbopack 구성과 5분 제한으로 재검사했다. 상대 경로 오류는 절대 경로 API 입력으로 우회했고 기본 C++ 및 STL의 컴파일·링크·실행을 확인했다. 직접 preset은 undefined imports를 사용하는 별도 경로이며 DALi strict 성공으로 처리하지 않는다.

특이 사항: lld 추가 라이브러리 사전 로딩 보강은 debug/WebGL2/pthread 정체를 해소했으나 실제 JS 생성 단계는 실패했다. 배포 runner가 compiler.mjs를 생략하고 가짜 최소 metadata를 반환하는 구현을 확인했다. 생략 해제·shebang 제거만으로도 실제 생성기는 실행되지 않으며 실패 subprocess를 상위 exit0으로 반환하는 경우도 재현했다. 출력 파일 부재와 stderr를 함께 판정했다. [결과와 개별 증거](EMCEPTION-5.0.7-FOLLOWUP.md).

5.0.7 채택·DALi 재빌드·전체 JS 실행 환경 포팅은 진행하지 않았다. 별도 운영 컴파일 서버를 도입하지 않는다. 기존 생산 입력497개 hash 일치, SDK·lock·기존 dirty tree 보존. 생산 코드 변경이 없어 공통/native 회귀 미재실행. P3/P4 GPU·격리·종료/재생성·Pages 승인 게이트 유지.


## 024 — 2026-10-26 원본 Emception 3.1.24와 DALi 버전 정렬

사용자 지시로 별도3.1.24 SDK에서 기존 Core·생산 GLES·D3 host를 재빌드했다. native-002 Core333·생산35 compile/nm·기본 WebGL2 strict 링크0, 원본 Emception browser-001 전체 생산 링크0, 새 사용자 C++ 컴파일/링크0 및 함수 반환5. 새 Core의 hash_memory 참조0으로 이전 혼용 실패가 해소됐다. [결과·증거](EMCEPTION-3.1.24-ALIGNMENT.md).

특이 사항: 초기 SDK archive .tar.xz 주소404는 공식 구형 .tbz2로 다운로드했다. 첫 빌드 도중 SDK 내부 병렬 제한 설정이 cache 재초기화를 유발해 표준 헤더 누락으로 실패했다. 설정을 고정하고 새 경로에서 처음부터 재빌드해 소스 수정 없이 통과했다. 실패 로그·기존 산출물 보존.

직접 COOP/COEP 및 헤더 없는 SW에서 같은 브라우저 생성 모듈 로딩·공유 메모리·함수 호출 성공. SW 최초 isolation=false에서 제어/reload 후 true. host 시작은 하지 않았고 실제 DALi 렌더/GPU·Worker GL·Pages·전체 종료/재생성은 미검증. 기존 CPU18/5모드의3.1.24 재실행 및 native build/runtime도 미실시. source GLES/Vulkan/DYNAMIC 회귀0, 기존 생산 입력497개 hash 일치. 기본 SDK lock6.0.13 유지, 생산 코드·원본 SDK·P0·과거 증거 보존. commit/push·배포 없음. 별도 운영 컴파일 서버 없는 방향과 P3/P4 게이트 유지.


## 025 — 2026-10-26 서버 없는 3.1.24 방향 채택·다음 컨텍스트 준비

사용자가 성공한 원본 Emception 3.1.24 정렬 방식을 채택하고 문서 갱신·인수인계 준비를 요청했다. [컴파일 방향](PLAYGROUND-COMPILATION.md)을 기준으로 PLAN의 외부 컴파일 서버 운영 문구를 제거했다. README·STATE·HANDOFF·BUILDING·NEXT-CONTEXT를 갱신했다. 기존 lock6.0.13은 과거 재현용으로 보존하고 다음 작업 SDK는3.1.24로 명시했다. 별도 프로필/lock 정리→CPU18/5 재검증→P3 실제 host/픽셀/GPU 검사 순서를 남겼다.

특이 사항: 방향 채택과 자동 도구 기본값 변경은 구분한다. 이번에는 문서만 수정했으며 코드·SDK·lock 변경, 새 빌드·브라우저·배포·commit/push는 하지 않았다. P2 링크 완료와 P3/P4·P8/P9 미완료를 유지한다. 실제 Pages 별도 승인 및 격리/전체 종료 게이트도 유지한다. 문서 상대 링크 검사·git diff --check 통과, 기존 생산 입력497개 hash 모두 일치했다. 코드 변경이 없어 빌드/native 회귀는 재실행하지 않는다.


## 2026-10-26 P3 최초 실제 렌더링

2026-10-26 후속 구현: [P3 최초 실제 렌더링 결과](P3-INITIAL-RESULTS.md). 별도 3.1.24 lock/명시 도구 선택, CPU18/5 재검증 완료. Web uniform 이름 길이 조회와 Core CPU uniform persistent mapping 재사용을 수정했다. 실제 Firefox pthread에서 직접 헤더/SW 모두 사각형·배경 두 프레임 픽셀/GL error0 및 ready→stopped를 확인했다. **P3 진행 중**: GPU 부분 갱신·callback·offset·grow/recycle·수명 종합 검사, 이벤트 Worker 격리, SW 후속 정책, Pages·전체 종료/재생성은 남는다. 최종 빌드 build/p3/pixels-3124-002와 docs/evidence/p3-initial을 따른다. 기존 lock/산출물 보존, commit/push/배포 없음.

특이 사항: Node 종료 옵션 누락, WebGL 미지원 이름 길이 조회 후 reflection trap, Core CPU uniform 중복 lock을 재현·수정했다. native configure는 compiler 부재로 실패하고13개 교차 compile/nm은 통과했다. 실패/중간 빌드를 보존하고 최종 소스는 새 빌드로 고정했다. [상세 검증·미검증 범위](P3-INITIAL-RESULTS.md).


## 2026-11-05 P3 GPU 메모리·reflection 실패 후속

[구현·검증·특이 사항·미검증](P3-GPU-MEMORY-RESULTS.md), [증거](evidence/p3-gpu-memory/README.md). 시작 HEAD/origin main04a686e9 일치·clean 확인. 승인된3.1.24 profile/lock·서버 없는 컴파일 방향 유지.

- 구현: 기본OFF 실제 생산 GPU 메모리 probe와 browser 판정 추가. GL 조회 실패 후 reflection이 결과를 사용하던 trap을 기존 CanSubmit gate로 수정. 공개 Graphics API·native Memory2/3·native retry 유지.
- 검증: 직접 헤더/SW 각각 두 프레임115개 검사·GPU46개 readback 일치·GL error0. 실제 scene CPU uniform 색 변화. shader/GL 실패 각각 ready→failure→stopped, 계측 없는 생산 host도 오류 없이 stopped. 최종499개 입력 drift0, 보존748개 변경0.
- 특이 사항: reflection 수정 전 trap·stopped 없음 재현. custom allocator는 기존 factory상 새 객체 생성이므로 일반 allocator recycle과 별도로 검사. 테스트 compile 오류·중간 hash drift 결과는 보존하고 최종35개 생산 객체를 새 경로에서 다시 컴파일했다.
- native: 현재 GNU11.4가 있어 configure3개·관련14개 native compile/nm0, 같은14개 Emscripten compile/nm0. 전체3개 native build는 설치된 Core 헤더 부재로1, runtime 미검증. 과거 compiler 부재와 다른 현재 장애다.
- 상태: GPU 메모리/재현 실패 단위 완료, P3 진행 중. 이벤트 Worker 격리·SW 후속 정책·P4 종료 전 무한 루프 응답성과 전체 pthread/Canvas 종료/재생성·별도 승인 Pages 유지. 정상 stopped로 전체 회수나 격리를 완료 처리하지 않는다. commit/push·upstream 갱신·운영 서버·배포 없음. 로컬 검증 프로세스는 종료했다.


## 2026-11-06 P3 GPU 메모리 결과 커밋·push 준비

사용자 요청에 따라 코드·검사·작은 증거·인계 문서를 한 커밋으로 정리했다. origin/main 읽기 조회는 시작 HEAD04a686e9와 일치했다. 최종 생산 입력499개 hash drift0, 직접 헤더/SW 메모리·shader/reflection 실패·계측 없는 생산 실행의 저장된 판정5개 통과, git diff --check를 확인했다. 생산 소스 변경이 없어 빌드/runtime 검사는 반복하지 않았다. SDK·산출물·프로필·큰 로그는 커밋에서 제외한다. push 완료 여부는 원격 SHA와 로컬 HEAD를 비교해 확인한다.
