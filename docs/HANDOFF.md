# 다음 context 인수인계: P2-a 최소 링크 대상·의존성 진단

2026-10-23. **D1~D4 확정, P1 설계 완료, P2 미착수.** 현재 기록은 `dali-web/docs`, 이전 `webassembly-probe`는 증거 보존 공간이다.

## 먼저 읽을 것

1. 상위 AGENTS.md, repo README, [STATE](STATE.md), [PLAN](PLAN.md).
2. [P1 실행 계약](P1-EXECUTION-CONTRACT.md), 특히 7절 D1~D4와 8절 P2 계획.
3. [P1 증거](evidence/p1/README.md), [R2-b 결과](R2-b-RESULTS.md).
4. `upstreams.lock.json`, `toolchain.lock.json`, [BUILDING](BUILDING.md), [UPSTREAMS](UPSTREAMS.md), [MIGRATION](MIGRATION.md), [Tizen 기준](TIZEN-BASELINE.md).

## 기준과 결과

작업 repo `/home/jy/work/dali/dali-web`, main, 원격 https://github.com/dalihub/dali-web. P1 시작 HEAD와 원격 main은 `ef6a1f545ff03df69c3297135048f545ee8dd352`로 일치했고 clean이었다. 최초 P1 문서 commit은 `d2964ae0910cc475992ed1c396247db150791b68`이다. D1 후속 문서 commit은 `Record approved GLES platform separation design`으로 식별한다. D2 후속 문서 commit은 `Record approved web buffer upload design`으로 식별한다. D3 후속 문서 commit은 `Record approved main-thread event execution design`이다. P1 마감 commit은 `Finalize P1 hosting policy and P2 handoff`로 식별한다. 실제 HEAD·status·remote를 다시 확인한다. upstream/lock은 갱신하지 않는다.

R2-a Core Wasm·Node, R2-b Firefox mock 50/100 및 Adaptor 6개 compile 통과. GL 탐색 Python 0, 두 링크 각각 1, 기본 7개/FULL_ES3 3개 unresolved. P1에서 이 검사를 반복하거나 생산 렌더링으로 승격하지 않았다.

P1은 현재 Core→GLES→표면/컨텍스트→업로드→프레임→종료 경로와 웹 경계를 기록했다. SDK 6.0.13의 map access guard만 추출해 Node 24.19.0에서 실행했고 exit 0이다. WRITE-only가 0으로 거부됨을 확인했으며 실제 GL 검사가 아니다. 새 로그는 `logs/p1/run-001/`, 작은 공개 증거는 `docs/evidence/p1/`이다.

## 확정된 결정과 남은 논의

1. D1 확정: 공통 GLES 로직과 Native/EGL·Web 환경 구현을 파일 단위로 분리해 빌드 시 선택한다. Core 계약과 기존 GLES/Vulkan/DYNAMIC 유지. 컨텍스트·표면, 업로드, sync/interop 책임을 분리하고 concrete EGL cast/타입을 native 파일로 이동한다. 단순 PoC보다 정식 포팅과 회귀 검증을 우선한다.
2. D2 확정: 별도 웹 메모리 구현(예: WebBufferMemory), 보존이 필요한 버퍼 소유 CPU shadow + BufferSubData. native Memory2/3 유지, Memory4 추가 없음. 명시적 구간 갱신과 callback을 구분하고 shadow 일관성·수명·오류·비용 검증. full overwrite만 허용하는 기능 축소안은 미채택.
3. D3 확정: 브라우저 main = DALi 이벤트, 별도 pthread Worker = 업데이트·렌더(E). DOM/이벤트 경계 분리와 main blocking 제거. C 이벤트 Worker는 플레이그라운드 격리 조기 검증 후보이며 지원 확정 아님. P2 링크 후 P3/P4 구간에서 조사·검증하고 P4 종료 전 결과·장애 기록.
4. D4 확정: GitHub Pages + SW 우선 검증, 실패/과도한 비용 시 직접 호스팅 전환 검토. 타사 관리형 호스팅 제외. D3 유지, 배포·서버 구축은 별도 승인. 최초 pthread/browser 검증 단위에 probe를 준비하고 실제 URL 검증 전 지원 확정 금지.

D1~D4는 모두 사용자 승인 완료다. P1의 호출 경로·파일 경계·기능 정책·스레드 선택·검증 방법 완료 조건을 충족했다. 실제 링크·픽셀·pthread·Pages는 검증 전이며 해당 단계 게이트에 남는다. 이번에는 문서 갱신 후 멈추고 다음 컨텍스트에서 P2-a를 실행한다.

## 다음 실행 단위 P2-a

현재 lock으로 생산 GLES source manifest와 Core/utility/GL/EGL/native 의존성 목록을 만들고 별도 build/log 디렉터리에서 컴파일·strict 링크 진단을 수행한다. 필요한 작은 CMake/진단 harness는 추가할 수 있지만, Controller 전체 분리·sync·shadow 구현은 이 진단 결과를 근거로 P2-b에 넘긴다. mock·빈 함수·미해결 심볼 억제로 성공 처리하지 않는다. 링크 실패도 개별 exit code와 원인을 보존한다. P2-a는 진단 완료와 구체적인 P2-b 파일/의존성/검증 계획을 인수 조건으로 하며, 생산 링크 성공 전에는 상위 P2 완료로 선언하지 않는다.

Pages/SW probe는 최초 pthread/browser 검증 단위에서 준비한다. P2-a에서 배포·서버 구축을 하지 않는다. D3 플레이그라운드 격리 probe는 P3/P4 구간에 별도로 검증한다. 예상 밖 범위 확대·합의 변경은 중단·보고한다.

## 특이 사항과 P2 경계

1. Memory3는 GLES3에서 자동 선택되나 WRITE-only가 SDK에서 거부된다. invalidate 추가만으로 partial-write 보존이 해결되지 않는다.
2. Memory2는 새 malloc staging 전체를 업로드한다. callback·uniform 부분 쓰기, map/lock 크기와 offset, 반복 unmap 수명을 검증해야 한다. GLES2로 버전을 위장하지 않는다.
3. EglGraphicsController.Initialize의 concrete EGL sync cast와 sync pool의 native FD 경로를 분리해야 한다. 빈 GL/EGL/sync 함수나 undefined-symbol 억제는 불가다.
4. 한 실제 WebGL context에서 resource/surface 역할과 캐시를 일치시킨다. SDK EGL 공유 인자를 native 공유 지원으로 오해하지 않는다.
5. P2 대상은 Core + 생산 GLES 자원/명령 구현 + 최소 web host다. 6개 R2-b object나 unused archive만 링크한 결과는 충분하지 않다. 전체 Tizen Adaptor/UI를 가져오지 않는다. 구체적인 source 집합·변경 파일·검증은 P1 8절에 있다.

P2 허가 후에는 별도 build/log dir, source manifest, nm/linker map, 실제 Controller/Core 연결, strict undefined-symbol 검사를 수행한다. unexpected dependency가 플랫폼 전체 포팅으로 커지면 중단·보고한다. P2 링크 exit 0과 P3 실제 픽셀 검증을 구분한다.

## 보존·미검증

원본 사용자 변경·P0·R2-a/R2-b 증거·lock·Core 기존 산출물을 보존했다. 외부 SDK는 선택적 재사용이며 개인 절대 경로를 필수 조건으로 만들지 않는다. 빌드 최대 -j6. SDK·산출물·브라우저 프로필·대용량 로그는 commit 제외. 남긴 서버·브라우저·빌드 프로세스 없음.

실제 DALi 픽셀·생산 GLES 링크/실행·전체 Adaptor·pthread/SAB/OffscreenCanvas·Pages headers/runtime·다른 브라우저/Windows·성능·native build/install·UI·context loss 복구는 미검증이다. [다음 프롬프트](NEXT-CONTEXT.md)는 P2-a를 실행하도록 구성했다.
