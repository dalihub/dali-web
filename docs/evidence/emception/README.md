# Emception 검증 증거

2026-10-26. [판정과 제한](../../EMCEPTION-ASSESSMENT.md).

- assessment-001: 공식 Pages에서 공개 합성 코드만 컴파일. 실제 버전·STL·문법 오류·pthread·WebGL2+OffscreenCanvas 결과. production 코드/입력을 외부 페이지에 전달하지 않았다.
- assessment-002: 공개 STL 프로그램을 컴파일한 뒤 별도 Worker에서 RESULT:3, exit 0.
- assessment-003: 오프라인 fixture 초기 시도. bridge 구문 오류 수정·새로고침 및 debug 라이브러리 누락 실패. host 객체 relocatable 링크만 0. 이 실패를 제품 호환성 결함으로 판정하지 않는다.
- assessment-004/005: STL 및 실제 DALi Vector2 사용자 코드 컴파일·현재 객체 링크·Worker 실행 0. 생산 링크는 libnoexit 로컬 배포 누락으로 실패. 자산 파서의 `$` 변수명 누락 수정 후 006에서 다시 검사했다.
- assessment-006: 누락 자산 없이 실제 Core archive·생산/host 35개 객체를 브라우저 Emception으로 strict 링크. exit 1, std::__2::__hash_memory 미해결. undefined 억제나 임의 대체 심볼 없음. nm으로 현재 Core의 참조와 구형 libc++의 정의 부재를 별도 확인했다.

명령·입력 소스·returncode·실행 메시지는 각 browser-tests.json, 버전은 environment.json, 실제 생산 입력 hash는 production-inputs.json이다. HTTP 로그는 loopback 파일만 기록한다. 최종 production 입력 497개 hash 변화 없음. 원본 SDK·lock·기존 증거·생산 코드 불변.

probe-*.py와 fixture-*는 실행한 harness 기록이다. `${DALI_WEB_ROOT}`와 `${DALI_WEB_EMSDK}`는 개인 절대 경로를 치환한 표기이며 그대로 실행하는 설치 스크립트가 아니다. 오프라인 실행 전 manifest의 정적 공개 파일을 별도 로컬 경로에 준비하고, production-inputs의 기존 객체를 복사해야 한다. 부족한 파일을 외부로 자동 요청하는 프록시는 사용하지 않는다. local probe는 127.0.0.1에만 bind하고 CSP와 Firefox의 외부 차단 프록시를 적용한다. 각 브라우저·driver·임시 서버는 검사 종료 후 닫는다.

제3자 페이지에 DALi 파일을 전달하는 초기 검사는 자동 승인 검토에서 거부되어 실행하지 않았다. 이후 공개 도구만 다운로드하고, DALi 검사는 외부 통신이 차단된 로컬 fixture에서 수행했다. 이 안전한 대안으로 해당 검사를 완료했으므로 외부 전달 승인은 필요하지 않다.

전체 배포물·객체·프로필·원본 소스 조회·nm 로그는 ignored logs/emception에 보존한다. 도구 다운로드는 배포 준비용 읽기 작업이며, 사용자 C++ 컴파일은 실제 브라우저 Wasm 도구에서 실행했다. Python은 브라우저 구동·정적 파일 제공·결과 수집만 담당했다. 서비스 컴파일 서버·Pages 배포·upstream 갱신·새 SDK 설치·LLVM 재빌드는 하지 않았다.
