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
