# 5.0.7 후보 예비 검사 증거

[판정](../../EMCEPTION-5.0.7-PRELIMINARY.md). `browser-NNN-result.json`은 브라우저에서 수집한 결과, `browser-NNN-probe.js`는 입력·명령·제한시간을 포함한 실행 fixture이다. 001은 argv 오류로 제외하며 002/004 미완료를 성공 코드로 대체하지 않는다. 002의 Firefox trap은 `browser-002-error.txt`, 003/004의 마지막 도구 활동은 result의 logs에 있다.

`manifest.json`은 공개 배포 manifest의 버전·fingerprint 요약 (전체 원본은 ignored 로그), `artifact-integrity.json`은 6개 도구 파일 hash 비교, `preservation.json`은 기존 생산 입력 보존 검사다. `harness-*.py`는 실행한 브라우저 제어 스크립트의 경로 정규화 사본이다. `${DALI_WEB_ROOT}`를 실제 checkout 경로로 바꾸고 npm 4.5.0 공개 배포물을 원래 디렉터리 배치로 준비해야 하며, 자동 설치 스크립트가 아니다. 각 probe.js를 해당 로그 루트의 probe.js로 배치하고 아직 없는 run 디렉터리를 사용한다.

전체 패키지·프로필·HTTP/driver 로그는 ignored `logs/emception/gameguild-001/`에 보존한다. 외부 페이지에 DALi 코드나 객체를 전달하지 않았으며 이번 입력은 합성 소스뿐이다. 임시 loopback 서버는 검증용 정적 파일 제공이며 별도 운영 컴파일 서버를 도입한 것이 아니다.
