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
