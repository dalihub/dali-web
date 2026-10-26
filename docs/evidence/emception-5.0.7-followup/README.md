# 5.0.7 추가 검사 증거

[판정과 제한](../../EMCEPTION-5.0.7-FOLLOWUP.md). 이번 검사는 공개 npm 배포물과 합성 입력만 사용했다. 전체 패키지·프로필·원본 HTTP/브라우저 로그는 ignored `logs/emception/gameguild-002..004`에 있다.

- `results.json`: 개별 브라우저 결과, 명령·입력·exitCode/timeout·파일 검사, 핵심 로그 발췌. timeout에는 도구 exitCode를 만들어 넣지 않는다. 상위 exit0이어도 stderr 오류와 산출물 부재가 있으면 실패다. Python harness 종료0은 검사 수집 완료만 의미한다.
- `environment.json`: 도구 버전과 원본/재설치 runner hash. `preservation.json`: 생산 입력497개 보존.
- `app-page.js`, `app-layout.js`, `next.config.mjs`, `package.json`: Next 최소 앱. upstream 설정 사본과 비교 가능하다. Next dev의 raw-loader/Worker 해석을 실제 사용했으며 공식 IDE 전체 UI를 복제하지 않았다.
- `harness-next.py`: Firefox 구동·결과 수집. 임시 Next 앱 주소127.0.0.1:3187 사용, 브라우저 외부 통신 차단. 마지막 브라우저가 종료된 뒤 Next도 종료했다.
- `gameguild-003-probe*.js`, `prewarm-change.txt`, `harness-prewarm*.py`: 원본과 별도 복사본의 lld prewarm 진단. 003의 core/xterm/CDN은 원본 공개 파일에 연결하고 browser만 복사한다.
- `gameguild-004-probe.js`, `harness-noskip.py`, `harness-noshebang.py`: browser 복사본의 `if (scriptPath.includes('compiler.mjs'))`를 false로 바꾸어 실제 JS 실행기를 거치게 한 진단. 두 번째는 `let processedCode = jsCode`에 선행 shebang 제거만 추가했다. 두 결과 모두 실패이며 stub으로 성공 처리한 것이 아니다.

재현 준비: 격리 디렉터리에서 기록된 package.json으로 `npm install --ignore-scripts --no-audit --no-fund`를 실행하고 공개 core/toolchain 4.5.0의 CDN을 `public/cdn`에 배치한다. `NEXT_TELEMETRY_DISABLED=1 RAYON_NUM_THREADS=6 node node_modules/next/dist/bin/next dev --hostname 127.0.0.1 --port 3187` 뒤 `python3 harness-next.py MODE`를 실행한다. MODE는 official, official-absolute, official-stl, optimized, debug, webgl, pthread다. Python harness의 `${DALI_WEB_ROOT}`는 실제 checkout 경로로 치환해야 한다. 각 run 디렉터리는 새로 준비하며 기존 증거를 덮어쓰지 않는다.

브라우저 Worker가 C++를 컴파일했다. Python/Next는 로컬 정적 파일과 앱 번들 제공·결과 수집만 담당했고 운영 컴파일 서버를 도입하지 않았다. Pages 배포, DALi/SDK 재빌드, 원본 lock 수정 없음. `-sERROR_ON_UNDEFINED_SYMBOLS=1`을 요청했지만 JS 생성 자체가 불완전하므로 Wasm 일부 생성만으로 최종 strict 링크 성공을 선언하지 않는다.
