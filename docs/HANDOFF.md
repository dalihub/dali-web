# 다음 context 인수인계: P1 결정 → P2

2026-10-23. **D1 아키텍처 확정, D2~D4 결정 대기. P1 완료 아님, P2 미착수.** 현재 기록은 `dali-web/docs`, 이전 `webassembly-probe`는 증거 보존 공간이다.

## 먼저 읽을 것

1. 상위 AGENTS.md, repo README, [STATE](STATE.md), [PLAN](PLAN.md).
2. [P1 실행 계약](P1-EXECUTION-CONTRACT.md), 특히 7절 D1~D4와 8절 P2 계획.
3. [P1 증거](evidence/p1/README.md), [R2-b 결과](R2-b-RESULTS.md).
4. `upstreams.lock.json`, `toolchain.lock.json`, [BUILDING](BUILDING.md), [UPSTREAMS](UPSTREAMS.md), [MIGRATION](MIGRATION.md), [Tizen 기준](TIZEN-BASELINE.md).

## 기준과 결과

작업 repo `/home/jy/work/dali/dali-web`, main, 원격 https://github.com/dalihub/dali-web. P1 시작 HEAD와 원격 main은 `ef6a1f545ff03df69c3297135048f545ee8dd352`로 일치했고 clean이었다. 최초 P1 문서 commit은 `d2964ae0910cc475992ed1c396247db150791b68`이다. D1 후속 문서 commit은 `Record approved GLES platform separation design`으로 식별한다. 실제 HEAD·status·remote를 다시 확인한다. upstream/lock은 갱신하지 않는다.

R2-a Core Wasm·Node, R2-b Firefox mock 50/100 및 Adaptor 6개 compile 통과. GL 탐색 Python 0, 두 링크 각각 1, 기본 7개/FULL_ES3 3개 unresolved. P1에서 이 검사를 반복하거나 생산 렌더링으로 승격하지 않았다.

P1은 현재 Core→GLES→표면/컨텍스트→업로드→프레임→종료 경로와 웹 경계를 기록했다. SDK 6.0.13의 map access guard만 추출해 Node 24.19.0에서 실행했고 exit 0이다. WRITE-only가 0으로 거부됨을 확인했으며 실제 GL 검사가 아니다. 새 로그는 `logs/p1/run-001/`, 작은 공개 증거는 `docs/evidence/p1/`이다.

## 확정된 결정과 남은 논의

1. D1 확정: 공통 GLES 로직과 Native/EGL·Web 환경 구현을 파일 단위로 분리해 빌드 시 선택한다. Core 계약과 기존 GLES/Vulkan/DYNAMIC 유지. 컨텍스트·표면, 업로드, sync/interop 책임을 분리하고 concrete EGL cast/타입을 native 파일로 이동한다. 단순 PoC보다 정식 포팅과 회귀 검증을 우선한다.
2. D2: CPU shadow + BufferSubData로 partial write 보존, 또는 full overwrite만 지원하고 callback/partial API 명시 제외.
3. D3: 생산 event Worker + update/render pthread(C) 권장, non-pthread 단일 Worker(B) 대안. main 단일 harness(A)는 링크 진단용으로만 제한하는 안.
4. D4: C이면 별도 격리 실행 origin을 허용할지, Pages 고정이면 B/SW 검증 중 어느 방향인지.

D1은 사용자 승인 완료이며 D2~D4는 미승인이다. 다음 논의는 D2의 부분 쓰기 보존과 CPU shadow 비용이다. D1 설계 승인만으로 생산 코드 구현을 시작하지 않는다. P1의 스레드 선택 완료 조건을 없애고 완료 처리하지 않는다. 실제 호스팅 사업자 선택은 배포 단계에 미룰 수 있지만 헤더 제어 허용 여부는 thread 결정과 함께 정해야 한다.

## 특이 사항과 P2 경계

1. Memory3는 GLES3에서 자동 선택되나 WRITE-only가 SDK에서 거부된다. invalidate 추가만으로 partial-write 보존이 해결되지 않는다.
2. Memory2는 새 malloc staging 전체를 업로드한다. callback·uniform 부분 쓰기, map/lock 크기와 offset, 반복 unmap 수명을 검증해야 한다. GLES2로 버전을 위장하지 않는다.
3. EglGraphicsController.Initialize의 concrete EGL sync cast와 sync pool의 native FD 경로를 분리해야 한다. 빈 GL/EGL/sync 함수나 undefined-symbol 억제는 불가다.
4. 한 실제 WebGL context에서 resource/surface 역할과 캐시를 일치시킨다. SDK EGL 공유 인자를 native 공유 지원으로 오해하지 않는다.
5. P2 대상은 Core + 생산 GLES 자원/명령 구현 + 최소 web host다. 6개 R2-b object나 unused archive만 링크한 결과는 충분하지 않다. 전체 Tizen Adaptor/UI를 가져오지 않는다. 구체적인 source 집합·변경 파일·검증은 P1 8절에 있다.

P2 허가 후에는 별도 build/log dir, source manifest, nm/linker map, 실제 Controller/Core 연결, strict undefined-symbol 검사를 수행한다. unexpected dependency가 플랫폼 전체 포팅으로 커지면 중단·보고한다. P2 링크 exit 0과 P3 실제 픽셀 검증을 구분한다.

## 보존·미검증

원본 사용자 변경·P0·R2-a/R2-b 증거·lock·Core 기존 산출물을 보존했다. 외부 SDK는 선택적 재사용이며 개인 절대 경로를 필수 조건으로 만들지 않는다. 빌드 최대 -j6. SDK·산출물·브라우저 프로필·대용량 로그는 commit 제외. 남긴 서버·브라우저·빌드 프로세스 없음.

실제 DALi 픽셀·생산 GLES 링크/실행·전체 Adaptor·pthread/SAB/OffscreenCanvas·Pages headers/runtime·다른 브라우저/Windows·성능·native build/install·UI·context loss 복구는 미검증이다. [다음 프롬프트](NEXT-CONTEXT.md)는 먼저 결정을 요청하도록 구성했다.
