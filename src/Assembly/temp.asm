;*
	Start of file
*;

.org		$1000
loadlabel:
		LDA	#$FFFF
		LDA	#$1234
.data
.outfile	"test"
datalabel:
.db		$01, $02, $03, $04
.db*		$01, $02, $03, $04
.d2b		$1234, $5678
.d2b*		$3412, $7856
.dw		$12345678,
.dw*		$78563412,
.d2w		$0123456789abcdef,
.d2w*		$efcdab8967452301,

; End