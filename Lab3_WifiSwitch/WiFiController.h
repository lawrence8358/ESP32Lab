#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif

// 前置宣告
class AsyncWebServer;

/** WiFi 自動設定管理類別 */
class WiFiController
{
public:
    /** 建構子 */
    WiFiController() = default;
    
    /** 連接 WiFi */
    bool connectWiFi();
    
    /** 日誌回調函式類型定義 */
    typedef void (*LogCallback)(String);    
    LogCallback logCallback;
    
    /** WiFi 資訊更新回調函式類型定義 */
    typedef void (*InfoCallback)();
    InfoCallback infoCallback;
    
    /** 清除 WiFi 設定 */
    void clearWiFiConfig();
    
    /** 取得 IP 位址 */
    String getIP() { return currentIP; }
    
    /** 取得 SSID */
    String getSSID() { return currentSSID; }
    
    /** 取得模式 (AP 或 STA) */
    String getMode() { return currentMode; }

private:
    /** 輸出日誌 */
    void printLog(String msg);
    
    /** 輸出日誌並換行 */
    void printlnLog(String msg) { printLog(msg + "\n"); }
    
    /** 輸出空白行 */
    void printlnLog() { printlnLog(""); }
    
    /** 嘗試連接已儲存的 WiFi */
    bool tryConnect();
    
    /** 初始化設定網頁伺服器 */
    void initConfigWeb();
    
    /** 啟動 Web 伺服器（連線成功後使用）*/
    void startWebServer();
    
    /** 設定 Web 伺服器路由（共用函數）*/
    void setupWebServer(AsyncWebServer* server, String& ssid, String& passwd, String& phase, String& message, bool& reboot, bool& reset);
    
    /** 處理設定網頁的主迴圈 */
    void handleConfigWebLoop(String& phase, String& message, String& ssid, String& passwd, String& wifiIp, bool& reboot, bool& reset);
    
    /** 嘗試連接指定的 WiFi 網路 */
    bool attemptWiFiConnection(const String& ssid, const String& passwd, String& wifiIp);
    
    /** 當前 IP 位址 */
    String currentIP;
    
    /** 當前 SSID */
    String currentSSID;
    
    /** 當前模式 (AP 或 STA) */
    String currentMode;
};

extern WiFiController WiFiCtrl;

const char wifi_config_html[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width,initial-scale=1" />
    <meta charset="utf-8" />
    <style>
        :root{--bg:#f5f7fb;--card:#fff;--accent:#007bff;--muted:#6c757d;--success:#28a745;--danger:#dc3545;--radius:12px}
        *{box-sizing:border-box}
        html,body{height:100%;margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI","Roboto","Noto Sans","Helvetica Neue",Arial,"微軟正黑體";background:linear-gradient(180deg,#e9f0ff,#f5f7fb)}
        .wrapper{min-height:100%;display:flex;align-items:center;justify-content:center;padding:20px}
        .card{width:100%;max-width:420px;background:var(--card);border-radius:var(--radius);box-shadow:0 6px 20px rgba(31,45,61,0.09);padding:20px}
        .header{text-align:center;margin-bottom:12px}
        .title{font-size:18px;font-weight:600;color:#10315a}
        .sub{font-size:13px;color:var(--muted)}
        .form{display:flex;flex-direction:column;gap:10px}
        .input{width:100%;padding:12px;border:1px solid #e3e8ef;border-radius:8px;font-size:15px}
        .row{display:flex;gap:8px}
        .btn{flex:1;padding:10px 12px;border-radius:8px;border:0;font-weight:600;font-size:15px;cursor:pointer}
        .btn-primary{background:var(--accent);color:#fff}
        .btn-outline{background:transparent;border:1px solid #cbd6ea;color:#10315a}
        .status{margin-top:12px;padding:10px;border-radius:8px;background:#f1f7ff;color:#034;display:flex;align-items:center;justify-content:space-between}
        .badge{font-size: 20px;}
        .badge-disconnected{color:var(--danger)}
        .badge-connected{color:var(--success)}
        .toggle-pass{display:flex;align-items:center;gap:8px}
        .btn-reset{background:#ea6b77;color:#fff;margin-top:20px;width:100%;}
        .btn-reset:hover{background:#a71d2a}
        @media (max-width:420px){.card{padding:16px;margin:0 6px}}
    </style>
</head>
<body>
    <div class="wrapper">
        <div class="card">
            <div class="header">
                <div>
                    <div class="title">無線網路設定</div>
                    <div class="sub" id="ssidInfo">載入中…</div>
                    <div class="sub" id="modeInfo" style="margin-top:4px;color:#6c757d;"></div>
                </div> 
            </div>

            <form class="form" action="/" method="POST" target="submitRes" onsubmit="return onSubmit(event)">
                <input class="input" name="ssid" id="inpSsid" placeholder="請輸入 Wi-Fi 名稱" autocomplete="off" />
                <div class="toggle-pass">
                    <input class="input" id="inpPass" name="passwd" type="password" placeholder="請輸入密碼" />
                    <button type="button" id="btnTogglePass" class="btn btn-outline" style="flex:0 0 auto;">顯示</button>
                </div>
                <div class="row">
                    <button type="submit" name="action" value="儲存設定" id="btnSave" class="btn btn-primary">儲存設定</button>
                    <button type="submit" name="action" value="重新啟動" id="btnReboot" class="btn btn-outline" disabled>重新啟動</button>
                </div>
            </form>
            <iframe name="submitRes" style="display:none"></iframe>

            <div id="statusBox" class="status" aria-live="polite">
                <div id="dvStatus">連線狀態：<span id="statusText">檢查中...</span></div>
                <div><span id="ipBadge" class="badge">●</span></div>
            </div>
            
            <form id="resetForm" action="/" method="POST" target="submitRes" onsubmit="return onResetSubmit(event)" style="display:none;">
                <button type="submit" name="action" value="還原預設值" class="btn btn-reset">還原預設值</button>
            </form>
        </div>
    </div>

    <script>
        function onSubmit(e){
            var action = e.submitter ? e.submitter.value : '';
            if(action === '儲存設定'){
                var ssid = document.getElementById('inpSsid').value.trim();
                var pass = document.getElementById('inpPass').value;
                if(!ssid || !pass){
                    alert('請輸入 Wi-Fi 名稱與密碼');
                    e.preventDefault();
                    return false;
                }
            }
            document.getElementById('statusText').innerHTML = '儲存中...';
            return true;
        }
        function onResetSubmit(e){
            if(!confirm('確定要還原預設值嗎？\n此操作將清除所有 Wi-Fi 設定並重新啟動裝置。')){
                e.preventDefault();
                return false;
            }
            document.getElementById('statusText').innerHTML = '正在還原預設值...';
            return true;
        }
        document.getElementById('btnTogglePass').addEventListener('click',function(){
            var p = document.getElementById('inpPass');
            if(p.type==='password'){p.type='text'; this.textContent='隱藏';}
            else{p.type='password'; this.textContent='顯示';}
        });

        var xhr = new XMLHttpRequest();
        var firstLoad = true;
        xhr.addEventListener('load', function(){ 
            var res = JSON.parse(this.responseText);
            console.log(res);
            document.getElementById('statusText').innerHTML = res.m || '';
            document.getElementById('ssidInfo').textContent = res.s ? '目前 Wi-Fi：'+res.s : '';
            document.getElementById('ipBadge').textContent = res.ip || '●';
            // 顯示模式資訊
            var modeText = res.mode === 'AP' ? '模式：AP (設定模式)' : '模式：STA (已連線)';
            document.getElementById('modeInfo').textContent = modeText;
            // 只在 STA 模式顯示還原預設值按鈕
            document.getElementById('resetForm').style.display = res.mode === 'STA' ? 'block' : 'none';
            // 在 STA 模式且首次載入時，自動填入已儲存的 WiFi 資訊
            if (firstLoad && res.mode === 'STA' && res.savedSsid) {
                document.getElementById('inpSsid').value = res.savedSsid;
                if (res.savedPasswd) {
                    document.getElementById('inpPass').value = res.savedPasswd;
                }
                firstLoad = false;
            }
            // CONNECTING && FAILED
            if(res.p === 'CONNECTED'){
                document.getElementById('btnSave').disabled = true;
                document.getElementById('btnReboot').disabled = false;
                document.getElementById('ipBadge').className='badge badge-connected';
            } else if(res.p === 'WAIT' || res.p === 'CONNECTING'){
                document.getElementById('ipBadge').className='badge';
            } else if(res.p === 'REBOOT'){
                clearInterval(hnd);
                document.getElementById('btnReboot').disabled = true; 
            } else if(res.p === 'RESET'){
                clearInterval(hnd);
                document.getElementById('btnSave').disabled = true;
                document.getElementById('btnReboot').disabled = true;
                // 5秒後自動導向預設 AP IP
                var apIP = res.apIP || '192.168.4.1';
                setTimeout(function(){
                    window.location.href = 'http://' + apIP;
                }, 5000);
            } else {
                document.getElementById('btnSave').disabled = false;
                document.getElementById('btnReboot').disabled = true;
                document.getElementById('ipBadge').className='badge badge-disconnected';
            } 
        });
        var hnd = setInterval(function(){ 
            xhr.open('GET','/status', true); 
            xhr.send(); 
        }, 1000);
    </script>
</body>
</html>
)====";
